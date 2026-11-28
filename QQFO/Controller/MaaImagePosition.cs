using System;

namespace MaaQQFO.Controller;

internal readonly record struct MaaImagePosition(int X, int Y)
{
    internal static MaaImagePosition? FromUniformPreview(double x, double y, double viewWidth, double viewHeight,
        double imageWidth, double imageHeight, int pixelWidth, int pixelHeight)
    {
        if (viewWidth <= 0 || viewHeight <= 0 || imageWidth <= 0 || imageHeight <= 0
            || pixelWidth <= 0 || pixelHeight <= 0)
            return null;

        var scale = Math.Min(viewWidth / imageWidth, viewHeight / imageHeight);
        var renderedWidth = imageWidth * scale;
        var renderedHeight = imageHeight * scale;
        var u = (x - (viewWidth - renderedWidth) / 2) / renderedWidth;
        var v = (y - (viewHeight - renderedHeight) / 2) / renderedHeight;
        if (!(u >= 0 && u < 1 && v >= 0 && v < 1))
            return null;

        return new(Math.Min((int)(u * pixelWidth), pixelWidth - 1), Math.Min((int)(v * pixelHeight), pixelHeight - 1));
    }
}
