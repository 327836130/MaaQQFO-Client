using MaaQQFO.Controller;
using Xunit;

namespace MFAAvalonia.Core.Tests;

public class QqfoWindowSelectionTests
{
    [Fact]
    public void KeepsSelectedWindowAcrossReorderAndTitleChange()
    {
        var selected = new QqfoWindowIdentity(1, 2, 3, 4);
        var other = selected with { Handle = 5 };
        Assert.Equal(1, QqfoWindowCandidate.FindSelection(
            [new(other, "QQ幻想"), new(selected, "新标题")], selected));
    }

    [Fact]
    public void RequiresExplicitSelectionAfterRestartOrWindowReplacement()
    {
        var previous = new QqfoWindowIdentity(1, 2, 3, 4);
        QqfoWindowCandidate[] replacements =
        [
            new(previous with { ProcessStartTicks = 5 }, "QQ幻想"),
            new(previous with { ThreadId = 6 }, "QQ幻想"),
            new(previous with { ProcessId = 7 }, "QQ幻想"),
            new(previous with { Handle = 8 }, "QQ幻想"),
        ];
        Assert.Equal(-1, QqfoWindowCandidate.FindSelection(replacements, previous));
        Assert.Equal(-1, QqfoWindowCandidate.FindSelection(replacements, null));
    }
}
