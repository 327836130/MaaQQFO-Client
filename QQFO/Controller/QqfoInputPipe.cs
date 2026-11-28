using System;
using System.Buffers.Binary;
using System.IO;
using System.IO.Pipes;
using System.Threading;
using System.Threading.Tasks;

namespace MaaQQFO.Controller;

// v1 fixed-width little-endian frame. No native pointers or packed runtime structs on the wire.
internal sealed class QqfoInputPipe(Guid session, uint pid, ulong hwnd) : IDisposable
{
    internal const int FrameSize = 56;
    private const uint Signature = 0x31464F51;
    private const int ReplyStatusOffset = 52;
    private NamedPipeClientStream? _pipe;
    private ulong _request;

    internal static string PipeName(Guid sessionId) => $"MaaQQFO.Input.{sessionId:N}";
    internal bool Connected => _pipe?.IsConnected == true;

    internal bool Connect()
    {
        Dispose();
        _pipe = new NamedPipeClientStream(".", PipeName(session), PipeDirection.InOut,
            PipeOptions.Asynchronous | PipeOptions.CurrentUserOnly);
        return Exchange(1, default, connect: true);
    }

    internal bool Click(ClientPosition position) => Connected && Exchange(2, position, connect: false);

    private bool Exchange(ushort operation, ClientPosition position, bool connect)
    {
        using var deadline = new CancellationTokenSource(TimeSpan.FromSeconds(2));
        try
        {
            var request = new byte[FrameSize];
            BinaryPrimitives.WriteUInt32LittleEndian(request, Signature);
            BinaryPrimitives.WriteUInt16LittleEndian(request.AsSpan(4), 1);
            BinaryPrimitives.WriteUInt16LittleEndian(request.AsSpan(6), operation);
            BinaryPrimitives.WriteUInt64LittleEndian(request.AsSpan(8), ++_request);
            session.TryWriteBytes(request.AsSpan(16, 16));
            BinaryPrimitives.WriteUInt64LittleEndian(request.AsSpan(32), hwnd);
            BinaryPrimitives.WriteUInt32LittleEndian(request.AsSpan(40), pid);
            BinaryPrimitives.WriteInt32LittleEndian(request.AsSpan(44), position.X);
            BinaryPrimitives.WriteInt32LittleEndian(request.AsSpan(48), position.Y);
            var reply = ExchangeAsync(_pipe!, request, connect, deadline.Token).GetAwaiter().GetResult();
            if (!request.AsSpan(0, ReplyStatusOffset).SequenceEqual(reply.AsSpan(0, ReplyStatusOffset)))
            {
                Dispose();
                return false;
            }
            var accepted = BinaryPrimitives.ReadInt32LittleEndian(reply.AsSpan(ReplyStatusOffset)) == 1;
            if (connect && !accepted)
                Dispose();
            return accepted;
        }
        catch (Exception error) when (error is IOException or UnauthorizedAccessException or OperationCanceledException)
        {
            // A lost reply is ambiguous. Break the session and never replay a Click.
            Dispose();
            return false;
        }
    }

    private static async Task<byte[]> ExchangeAsync(NamedPipeClientStream pipe, byte[] request, bool connect, CancellationToken token)
    {
        if (connect)
            await pipe.ConnectAsync(token).ConfigureAwait(false);
        await pipe.WriteAsync(request, token).ConfigureAwait(false);
        var reply = new byte[FrameSize];
        await pipe.ReadExactlyAsync(reply, token).ConfigureAwait(false);
        return reply;
    }

    public void Dispose()
    {
        _pipe?.Dispose();
        _pipe = null;
    }
}
