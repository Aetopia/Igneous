using System;
using System.Runtime.InteropServices;

static class Igneous
{
    [DllImport("Igneous")]
    static extern void Igneous_Library_Initialize();

    [DllImport("Igneous")]
    static extern void Igneous_Library_Uninitialize();

    [DllImport("Igneous")]
    static extern int Igneous_Game_Launch(nint @this, out int value);

    [DllImport("Igneous")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern bool Igneous_App_get_Installed(nint @this);

    [DllImport("Igneous")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern bool Igneous_App_get_Running(nint @this);

    [DllImport("Kernel32", CharSet = CharSet.Ansi)]
    static extern nint GetProcAddress(nint hModule, string lpProcName);

    [DllImport("Kernel32", CharSet = CharSet.Unicode)]
    static extern nint GetModuleHandle(string lpModuleName);

    static readonly nint Igneous_Minecraft_Release;

    static Igneous()
    {
        Igneous_Library_Initialize();

        var module = GetModuleHandle("Igneous");
        Igneous_Minecraft_Release = GetProcAddress(module, nameof(Igneous_Minecraft_Release));

        AppDomain.CurrentDomain.ProcessExit += (_, _) => Igneous_Library_Uninitialize();
    }

    internal static void Launch()
    {
        Console.WriteLine($"Minecraft is {(Igneous_App_get_Installed(Igneous_Minecraft_Release) ? "installed" : "not installed.")}.");
        Console.WriteLine($"Minecraft is {(Igneous_App_get_Running(Igneous_Minecraft_Release) ? "running" : "not running")}.");
        //  _ = Igneous_Game_Launch(Igneous_Minecraft_Release, out _);
    }
}