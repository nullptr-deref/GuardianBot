#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <imgui.h>

#include <spdlog/spdlog.h>

#include "Serial/SerialPort.hpp"
#include "ImGuiConstants.hpp"

void clearBuffer(char *buf, const size_t bsize);

namespace wnd {
    void showWatcherWindow(size_t humanCount) {
        bool watcherShown = true;
        ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
        ImGui::SetNextWindowSize({ imguic::watcher::w, imguic::watcher::h }, ImGuiCond_Always);
        ImGui::Begin("watcher", &watcherShown);
        {
            const std::string infoLabel = "Currently watching %u " + std::string(humanCount == 1 ? "person" : "persons");

            ImGui::BeginChild("Output");
            ImGui::Text(infoLabel.c_str(), humanCount);
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void showControllerWindow(std::unique_ptr<SerialPort> &port, char *commandBuf, size_t bufSize, const std::vector<std::string> &ports) {
        bool controllerShown = true;
        static std::string connectedPortName = "";

        ImGui::SetNextWindowPos({ imguic::controller::x, imguic::controller::y }, ImGuiCond_Always);
        ImGui::SetNextWindowSize({ imguic::controller::w, imguic::controller::h }, ImGuiCond_Always);
        static std::string sendMessage;
        ImGui::Begin("controller", &controllerShown);
            ImGui::BeginChild("port", ImVec2(0, 64), true);
            if (ImGui::BeginMenu("Available COM ports")) {
                for (const auto &availablePort: ports) {
                    if (ImGui::MenuItem(availablePort.c_str())) {
                        port = std::make_unique<SerialPort>(availablePort, SerialMode::ReadWrite);
                        connectedPortName = availablePort;
                    }
                }
                ImGui::EndMenu();
            }
            const std::string connectionLabel = !connectedPortName.empty() ?
                "Currently connected to " + connectedPortName + "." :
                "No COM port connection.";
            ImGui::Text(connectionLabel.c_str());
            ImGui::EndChild();

            ImGui::BeginChild("commands", ImVec2(0, 0), true);
            ImGui::InputText("Type a command", commandBuf, bufSize);
            if (ImGui::Button("Send", { imguic::controller::btnW, imguic::controller::btnH }))
            {
                try {
                    if (connectedPortName.empty()) {
                        sendMessage = "No COM port selected to send the command.";
                        spdlog::warn("No COM port selected to send the command");
                    }
                    else {
                        port->open();
                        sendMessage = "Trying to send command to COM port...";

                        spdlog::info("Trying to send command to COM port");
                        port->write(commandBuf, static_cast<uint32_t>(bufSize));
                        spdlog::info("Wrote to COM port successfully.");
                        sendMessage = "Wrote to COM port successfully.";
                        port->close();
                        clearBuffer(commandBuf, bufSize);
                    }
                }
                catch (const std::runtime_error &e) {
                    spdlog::warn(e.what());
                }
            }
            ImGui::Text(sendMessage.c_str());
            ImGui::EndChild();
        ImGui::End();
    }
}

void clearBuffer(char *buf, const size_t bsize) {
    for (size_t i = 0; i < bsize; i++) buf[i] = 0;
}