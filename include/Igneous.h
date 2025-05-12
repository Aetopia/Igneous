#pragma once

#include <winerror.h>
#include <minwindef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*
        Check if a game is installed.
        [In] Handle to a game.
    */

    BOOL WINAPI Game_get_Installed(PVOID);

    /*
        Check if a game is running.
        [In] Handle to a game.
    */

    BOOL WINAPI Game_get_Running(PVOID);

    /*
        Launch a game.
        [In] Handle to a game.
        [Out] The process identifer.
    */

    HRESULT WINAPI Game_Launch(PVOID, PDWORD);

    /*
        Configure debug mode for a game.
        [In] Handle to a game.
        [In] TRUE to enable or FALSE to disable.
    */

    HRESULT WINAPI Game_set_Debug(PVOID, BOOL);

    /*
        Terminate a game.
        [In] Handle to a game.
    */

    HRESULT WINAPI Game_Terminate(PVOID);

    /*
        Returns a handle for Minecraft.
    */

    PVOID WINAPI Minecraft_get_Release();

    /*
        Returns a handle for Minecraft: Preview.
    */

    PVOID WINAPI Minecraft_get_Preview();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <optional>
#include <winrt/base.h>

/*
    Represents a game.
*/

class Game
{
  private:
    PVOID _This;

  public:
    Game(PVOID This)
    {
        _This = This;
    }

    /*
        Check if a game is installed.
    */

    bool Installed()
    {
        return Game_get_Installed(_This);
    }

    /*
        Check if a game is running.
    */

    bool Running()
    {
        return Game_get_Running(_This);
    }

    /*
        Launch a game.
        [Returns] The process identifer.
    */

    std::optional<DWORD> Launch()
    {
        DWORD dwProcessId = {};
        HRESULT hResult = Game_Launch(_This, &dwProcessId);

        if (hResult == S_FALSE)
            return {};
        else
            winrt::check_hresult(hResult);

        return dwProcessId;
    }

    /*
       Configure debug mode for a game.
       [In] TRUE to enable or FALSE to disable.
   */

    void Debug(bool value)
    {
        winrt::check_hresult(Game_set_Debug(_This, value));
    }

    /*
        Terminate a game.
    */

    void Terminate()
    {
        winrt::check_hresult(Game_Terminate(_This));
    }
};

/*
    Provides access to Minecraft: Bedrock Edition.
*/

class Minecraft
{
  private:
    static inline Game _Release{Minecraft_get_Release()}, _Preview{Minecraft_get_Preview()};

  public:
    /*
        Provides access to Minecraft.
    */

    static Game &Release() noexcept
    {
        return _Release;
    }

    /*
        Provides access to Minecraft: Preview.
    */

    static Game &Preview() noexcept
    {
        return _Preview;
    }
};

#endif