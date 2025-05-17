#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

template <typename... Args>
class EventManager {
public:
    using EventCallback = std::function<void(Args...)>;

    void AddListener(const std::string& eventName, EventCallback callback) {
        extra_events[eventName].push_back(callback);
    }

    void RemoveListener(const std::string& eventName, EventCallback callback) {
        // TODO: test
        auto it = extra_events.find(eventName);
        if (it != extra_events.end()) {
            auto& callbacks = it->second;
            callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
                [&callback](const EventCallback& existingCallback) {
                    // Compare the target of the function objects
                    return existingCallback.target_type() == callback.target_type();
                }),
                callbacks.end());

            // if no callbacks remain, remove the event from the map
            if (callbacks.empty()) {
                extra_events.erase(it);
            }
        }
    }

    // Dispatch an event with arguments
    void Dispatch(const std::string& eventName, Args... args) {
        if (extra_events.find(eventName) != extra_events.end()) {
            for (const auto& callback : extra_events[eventName]) {
                callback(args...);
            }
        }
        else {
            std::cout << "No listeners for event: " << eventName << std::endl;
        }
    }

private:
    std::map<std::string, std::vector<EventCallback>> extra_events;
};