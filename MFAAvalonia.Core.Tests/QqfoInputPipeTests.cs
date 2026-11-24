using MaaQQFO.Controller;
using System;
using System.Buffers.Binary;
using System.IO.Pipes;
using System.Threading;
using System.Threading.Tasks;
using Xunit;

namespace MFAAvalonia.Core.Tests;

public class QqfoInputPipeTests
{
    [Theory]
    [InlineData(false)]
    [InlineData(true)]
    public async Task ValidatesSessionAndRejectsMismatchedReply(bool changeRequestId)
    {
        if (!OperatingSystem.IsWindows())
            return;
        var session = Guid.NewGuid();
        using var server = new NamedPipeServerStream(QqfoInputPipe.PipeName(session), PipeDirection.InOut,
            1, PipeTransmissionMode.Byte, PipeOptions.Asynchronous | PipeOptions.CurrentUserOnly);
        using var client = new QqfoInputPipe(session, 123, 456);
        using var timeout = new CancellationTokenSource(TimeSpan.FromSeconds(10));
        var host = Task.Run(async () =>
        {
            await server.WaitForConnectionAsync(timeout.Token);
            var frame = new byte[QqfoInputPipe.FrameSize];
            await server.ReadExactlyAsync(frame, timeout.Token);
            Assert.Equal(0x31464F51u, BinaryPrimitives.ReadUInt32LittleEndian(frame));
            Assert.Equal(1, BinaryPrimitives.ReadUInt16LittleEndian(frame.AsSpan(4)));
            Assert.Equal(1, BinaryPrimitives.ReadUInt16LittleEndian(frame.AsSpan(6)));
            Assert.Equal(session, new Guid(frame.AsSpan(16, 16)));
            Assert.Equal(456ul, BinaryPrimitives.ReadUInt64LittleEndian(frame.AsSpan(32)));
            Assert.Equal(123u, BinaryPrimitives.ReadUInt32LittleEndian(frame.AsSpan(40)));
            BinaryPrimitives.WriteInt32LittleEndian(frame.AsSpan(52), 1);
            await server.WriteAsync(frame.AsMemory(0, 13), timeout.Token);
            await server.WriteAsync(frame.AsMemory(13), timeout.Token);
            await server.ReadExactlyAsync(frame, timeout.Token);
            Assert.Equal(2, BinaryPrimitives.ReadUInt16LittleEndian(frame.AsSpan(6)));
            Assert.Equal(2ul, BinaryPrimitives.ReadUInt64LittleEndian(frame.AsSpan(8)));
            Assert.Equal(800, BinaryPrimitives.ReadInt32LittleEndian(frame.AsSpan(44)));
            Assert.Equal(450, BinaryPrimitives.ReadInt32LittleEndian(frame.AsSpan(48)));
            if (changeRequestId)
                frame[8]++;
            BinaryPrimitives.WriteInt32LittleEndian(frame.AsSpan(52), 1);
            await server.WriteAsync(frame, timeout.Token);
        }, timeout.Token);
        Assert.True(client.Connect());
        Assert.Equal(!changeRequestId, client.Click(new ClientPosition(800, 450)));
        if (changeRequestId)
        {
            Assert.False(client.Connected);
            Assert.False(client.Click(new ClientPosition(800, 450)));
        }
        await host;
    }

    [Fact]
    public void MissingHostFailsWithABoundedWait()
    {
        if (!OperatingSystem.IsWindows())
            return;
        using var client = new QqfoInputPipe(Guid.NewGuid(), 123, 456);
        var started = System.Diagnostics.Stopwatch.StartNew();
        Assert.False(client.Connect());
        Assert.False(client.Connected);
        Assert.True(started.Elapsed < TimeSpan.FromSeconds(8));
    }
}
