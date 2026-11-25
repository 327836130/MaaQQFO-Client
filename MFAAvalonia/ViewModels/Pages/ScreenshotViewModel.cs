using Avalonia.Media;
using Avalonia.Platform.Storage;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MFAAvalonia.Configuration;
using MFAAvalonia.Extensions;
using MFAAvalonia.Extensions.MaaFW;
using MFAAvalonia.Helper;
using MFAAvalonia.Helper.ValueType;
using System;
using System.IO;
using System.Threading.Tasks;
using Bitmap = Avalonia.Media.Imaging.Bitmap;

namespace MFAAvalonia.ViewModels.Pages;

public partial class ScreenshotViewModel : ViewModelBase
{
#pragma warning disable CS4014 // Because this call is not awaited, execution of the current method continues before the call is completed
    [ObservableProperty]
    [NotifyPropertyChangedFor(nameof(ScreenshotPixelSize))]
    private Bitmap? _screenshotImage;
    public string? ScreenshotPixelSize => ScreenshotImage is { PixelSize: var pixels }
        ? $"{pixels.Width} × {pixels.Height} px"
        : null;
    [ObservableProperty] private string _taskName = string.Empty;
    [RelayCommand]
    private async Task Screenshot()
    {
        try
        {
            var vm = Instances.InstanceTabBarViewModel.ActiveTab?.TaskQueueViewModel;
            if (vm == null) return;
            if (vm.Processor.UsesQqfoInputSession)
            {
                var target = vm.Processor.Config.DesktopWindow.QqfoTarget;
                var tasker = vm.Processor.MaaTasker;
                var previous = ScreenshotImage;
                ScreenshotImage = null;
                previous?.Dispose();
                TaskName = string.Empty;
                if (target == null || tasker == null || !vm.IsConnected)
                {
                    ToastHelper.Warn(LangKeys.Window.ToLocalization(), LangKeys.Unconnected.ToLocalization());
                    return;
                }
                var image = await Task.Run(vm.Processor.CaptureQqfoTestScreenshot);
                if (Instances.InstanceTabBarViewModel.ActiveTab?.TaskQueueViewModel != vm
                    || vm.Processor.Config.DesktopWindow.QqfoTarget != target
                    || !ReferenceEquals(tasker, vm.Processor.MaaTasker))
                {
                    image?.Dispose();
                    return;
                }
                if (image == null)
                    ToastHelper.Warn(LangKeys.ScreenshotFailed.ToLocalization());
                else
                {
                    var replaced = ScreenshotImage;
                    ScreenshotImage = image;
                    replaced?.Dispose();
                }
                return;
            }

            if (vm.Processor.MaaTasker is not { IsInitialized: true })
            {
                ToastHelper.Info(LangKeys.Tip.ToLocalization(), LangKeys.ConnectingTo.ToLocalizationFormatted(true, vm.CurrentController == MaaControllerTypes.Adb ? "Emulator" : "Window"));
                vm.Processor.TaskQueue.Enqueue(new MFATask
                {
                    Name = "截图前启动",
                    Type = MFATask.MFATaskType.MFA,
                    Action = async () => await vm.Processor.TestConnecting(),
                    OwnerViewModel = vm,
                });
                vm.Processor.TaskQueue.Enqueue(new MFATask
                {
                    Name = "截图任务",
                    Type = MFATask.MFATaskType.MFA,
                    Action = async () => await TaskManager.RunTaskAsync(() =>
                    {
                        var bitmap = vm.Processor.GetLiveView();
                        if (bitmap == null)
                            ToastHelper.Warn(LangKeys.ScreenshotFailed.ToLocalization());

                        DispatcherHelper.PostOnMainThread((() =>
                        {
                            var oldImage = ScreenshotImage;
                            ScreenshotImage = bitmap;
                            oldImage?.Dispose();
                            TaskName = string.Empty;
                        }));
                    }, name: "截图测试"),
                    OwnerViewModel = vm,
                });
                vm.Processor.Start(true, checkUpdate: false);

            }
            else
                TaskManager.RunTaskAsync(() =>
                {
                    var bitmap = vm.Processor.GetLiveViewCached();
                    if (bitmap == null)
                        ToastHelper.Warn(LangKeys.ScreenshotFailed.ToLocalization());

                    DispatcherHelper.PostOnMainThread((() =>
                    {
                        var oldImage = ScreenshotImage;
                        ScreenshotImage = bitmap;
                        oldImage?.Dispose();
                        TaskName = string.Empty;
                    }));
                }, name: "截图测试");
        }
        catch (Exception e)
        {
            LoggerHelper.Error($"执行截图流程失败：原因={e.Message}", e);
        }
    }

    [RelayCommand]
    private async Task SaveScreenshot()
    {
        if (ScreenshotImage is not { } image)
        {
            ToastHelper.Warn(LangKeys.Warning.ToLocalization(), LangKeys.ScreenshotEmpty.ToLocalization());
            return;
        }
        var options = new FilePickerSaveOptions
        {
            Title = LangKeys.SaveScreenshot.ToLocalization(),
            FileTypeChoices =
            [
                new FilePickerFileType("PNG")
                {
                    Patterns = ["*.png"]
                }
            ]
        };

        var storageProvider = Instances.StorageProvider;
        if (storageProvider == null)
        {
            ToastHelper.Warn(LangKeys.Warning.ToLocalization(), LangKeys.PlatformNotSupportedOperation.ToLocalization());
            return;
        }

        using var png = new MemoryStream();
        image.Save(png);
        png.Position = 0;
        if (await storageProvider.SaveFilePickerAsync(options) is not { } result || result.TryGetLocalPath() is not { } path)
            return;

        await using var stream = File.Create(path);
        await png.CopyToAsync(stream);
    }
}
