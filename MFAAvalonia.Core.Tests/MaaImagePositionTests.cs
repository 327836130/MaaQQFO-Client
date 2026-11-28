using MaaQQFO.Controller;
using Xunit;

namespace MFAAvalonia.Core.Tests;

public class MaaImagePositionTests
{
    [Theory]
    [InlineData(160, 120, 320, 240)]
    [InlineData(0, 0, 0, 0)]
    [InlineData(319.5, 239.5, 639, 479)]
    public void MapsPreviewDipsToMaaImagePixels(double x, double y, int expectedX, int expectedY)
    {
        Assert.Equal(new MaaImagePosition(expectedX, expectedY),
            MaaImagePosition.FromUniformPreview(x, y, 320, 240, 640, 480, 640, 480));
    }

    [Fact]
    public void RejectsLetterboxAndExclusiveRightEdge()
    {
        Assert.Equal(new MaaImagePosition(0, 0), MaaImagePosition.FromUniformPreview(40, 0, 400, 240, 640, 480, 640, 480));
        Assert.Null(MaaImagePosition.FromUniformPreview(39, 120, 400, 240, 640, 480, 640, 480));
        Assert.Null(MaaImagePosition.FromUniformPreview(360, 120, 400, 240, 640, 480, 640, 480));
    }
}
