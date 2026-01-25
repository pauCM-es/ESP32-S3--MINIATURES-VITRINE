#pragma once

class MaintenanceMode {
public:
    static MaintenanceMode& getInstance();
    
    void enter();
    void exit();
    bool isActive() const;
    
    // Check if maintenance button is held at boot
    static bool checkBootTrigger();

private:
    MaintenanceMode();
    MaintenanceMode(const MaintenanceMode&) = delete;
    MaintenanceMode& operator=(const MaintenanceMode&) = delete;
    
    bool active;
};
