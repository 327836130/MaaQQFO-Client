using MaaQQFO.Controller;
using System;
using Xunit;

namespace MFAAvalonia.Core.Tests;

public class QqfoControllerCallbacksTests
{
    [Fact]
    public void ClickUsesClientCoordinatesUnchangedAndRejectsDisconnectedInput()
    {
        var backend = new Backend();
        using var callbacks = new QqfoControllerCallbacks("owned-test-session", backend);
        Assert.False(callbacks.Click(800, 450));
        Assert.True(callbacks.Connect());
        Assert.False(callbacks.Click(-1, 450));
        Assert.True(callbacks.Click(800, 450));
        Assert.Equal(new ClientPosition(800, 450), backend.Clicked);
        Assert.False(callbacks.ClickKey(65));
    }

    [Fact]
    public void ReleasedCallbackCannotReuseSession()
    {
        var backend = new Backend();
        var callbacks = new QqfoControllerCallbacks("owned-test-session", backend);
        Assert.True(callbacks.Connect());
        callbacks.Dispose();
        callbacks.Dispose();
        Assert.Equal(1, backend.DisposeCount);
        Assert.False(callbacks.Connected());
        Assert.False(callbacks.Connect());
        Assert.False(callbacks.Click(0, 0));
        using var replacement = new QqfoControllerCallbacks("replacement-session", new Backend());
        Assert.True(replacement.Connect());
        Assert.True(replacement.Click(0, 0));
    }

    [Fact]
    public void BackendFailureIsReportedAsFalseAtTheNativeBoundary()
    {
        var backend = new Backend { ThrowOnClick = true };
        using var callbacks = new QqfoControllerCallbacks("owned-test-session", backend);
        Assert.True(callbacks.Connect());
        Assert.False(callbacks.Click(800, 450));
        Assert.Null(backend.Clicked);
    }

    private sealed class Backend : IQqfoControllerBackend
    {
        public bool Connected { get; private set; }
        public ClientPosition? Clicked { get; private set; }
        public bool ThrowOnClick { get; init; }
        public int DisposeCount { get; private set; }
        public bool Connect() => Connected = true;
        public bool TryCapture(out ReadOnlyMemory<byte> clientImage)
        {
            clientImage = default;
            return false;
        }
        public bool Click(ClientPosition position)
        {
            if (ThrowOnClick)
                throw new InvalidOperationException("fixture failure");
            Clicked = position;
            return true;
        }
        public void Dispose()
        {
            ++DisposeCount;
            Connected = false;
        }
    }
}
