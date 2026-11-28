using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using MaaQQFO.Controller;
using MFAAvalonia.Helper;

namespace MFAAvalonia.Views.Pages;

public partial class ScreenshotView : UserControl
{
    public ScreenshotView()
    {
        DataContext = Instances.ScreenshotViewModel;
        InitializeComponent();
        RenderOptions.SetBitmapInterpolationMode(image, BitmapInterpolationMode.HighQuality);
        image.PropertyChanged += (_, change) =>
        {
            if (change.Property == Image.SourceProperty)
                ImagePositionText.Text = null;
        };
    }

    private void InspectImagePosition(object? sender, PointerPressedEventArgs args)
    {
        if (image.Source is not Bitmap bitmap)
            return;

        var pointer = args.GetPosition(image);
        var position = MaaImagePosition.FromUniformPreview(pointer.X, pointer.Y, image.Bounds.Width, image.Bounds.Height,
            bitmap.Size.Width, bitmap.Size.Height, bitmap.PixelSize.Width, bitmap.PixelSize.Height);
        ImagePositionText.Text = position is { } point ? $"MaaImage ({point.X}, {point.Y})" : null;
    }
}
