#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <imgui.h>

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
        static std::string currentlyConnectedPort;

        ImGui::SetNextWindowPos({ imguic::controller::x, imguic::controller::y }, ImGuiCond_Always);
        ImGui::SetNextWindowSize({ imguic::controller::w, imguic::controller::h }, ImGuiCond_Always);
        ImGui::Begin("controller", &controllerShown);
            ImGui::BeginChild("port", ImVec2(0, 64), true);
            if (ImGui::BeginMenu("Available COM ports"))
            {
                for (const auto &iport : ports)
                {
                    if (ImGui::MenuItem(iport.c_str()))
                    {
                        port = std::make_unique<SerialPort>(iport, SerialMode::ReadWrite);
                        currentlyConnectedPort = iport;
                    }
                }
                ImGui::EndMenu();
            }
            const std::string connectionLabel = currentlyConnectedPort.size() > 0 ?
                "Currently connected to " + currentlyConnectedPort + "." :
                "No COM port connection.";
            ImGui::Text(connectionLabel.c_str());
            ImGui::EndChild();

            ImGui::BeginChild("commands", ImVec2(0, 0), true);
            ImGui::InputText("Type a command", commandBuf, bufSize);
            if (ImGui::Button("Send", { imguic::controller::btnW, imguic::controller::btnH }))
            {
                port->open();
                std::clog << "[PORT INFO] Sending: ";
                for (uint32_t i = 0; i < bufSize; i++) {
                    if (commandBuf[i] == 0) break;
                    std::clog << commandBuf[i];
                }
                std::clog << '\n';

                port->write(commandBuf, bufSize);
                std::clog << "Wrote to port successfully.\n";
                port->close();
                clearBuffer(commandBuf, bufSize);
            }
            ImGui::EndChild();
        ImGui::End();
    }
}

void clearBuffer(char *buf, const size_t bsize)
{
    for (size_t i = 0; i < bsize; i++) buf[i] = 0;
}