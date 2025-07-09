#include "Shared.h"
#include "Globals.h"

#include <iostream>
#include <filesystem>
#include <unordered_set>

std::unordered_set<IPlugin*> Plugins;

void loadPlugins() {
    if (!FwmodConfig.loadPlugins) {
        CoreLogger.Info("plugin loading disabled. skipping...");
        return;
    }
    if (!std::filesystem::exists(pluginsPath)) {
        CoreLogger.Error("The specified path does not exist: " + std::string(pluginsPath));
        return;
    }

    CoreLogger.Info("loading plugins from " + std::string(pluginsPath) + "!");
    for (const auto& entry : std::filesystem::directory_iterator(pluginsPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dll") {
            std::string dllPath = entry.path().string();
            HMODULE hModule = LoadLibraryA(dllPath.c_str());
            if (!hModule) {
                CoreLogger.Info("Failed to load " + dllPath + ": " + std::to_string(GetLastError()));
                return;
            }

            CreatePluginFunc createPlugin = (CreatePluginFunc)GetProcAddress(hModule, "CreatePlugin");
            if (!createPlugin) {
                CoreLogger.Info("Failed to find CreatePlugin function in: " + dllPath);
                return;
            }

            IPlugin* plugin = createPlugin();
            if (!plugin) {
                CoreLogger.Info("Failed to create plugin from: " + dllPath);
                return;
            }
            plugin->Initialize();
            Plugins.insert(plugin);
            CoreLogger.Info("Loaded plugin: " + std::string(plugin->GetName()));
        }
    }
}

void unloadPlugins() {
    for (IPlugin* plugin : Plugins) {
        PluginFlags request = plugin->Shutdown();
        HMODULE handle = plugin->hModule;

        delete plugin;

        switch (request) {
            case PluginFlags::UnloadDll:
                FreeLibrary(handle);
        }
    }
    Plugins.clear();
}
