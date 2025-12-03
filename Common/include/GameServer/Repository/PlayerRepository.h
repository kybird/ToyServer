#pragma once
#include "GameServer/Repository/IRepository.h"
#include "GameServer/Database/ISqlDatabase.h"
#include <string>
#include <iostream>

namespace GameServer::Repository {

struct Player {
    int id;
    std::string name;
    int level;
};

class PlayerRepository : public IRepository<Player, int> {
public:
    PlayerRepository(std::shared_ptr<Database::ISqlDatabase> db) : _db(db) {}

    bool Save(const Player& player) override {
        // In a real scenario, we'd construct a SQL INSERT/UPDATE statement
        std::string query = "INSERT INTO Players (id, name, level) VALUES (" + 
                            std::to_string(player.id) + ", '" + player.name + "', " + 
                            std::to_string(player.level) + ")";
        return _db->Execute(query);
    }

    std::optional<Player> Find(int key) override {
        // For Find, we need a way to get results from IDatabase.
        // Since we haven't fully implemented ResultSets yet, we'll mock the return.
        std::cout << "[Mock] Finding Player with ID: " << key << std::endl;
        
        // Mock return
        Player p;
        p.id = key;
        p.name = "TestPlayer";
        p.level = 1;
        return p;
    }

    bool Delete(int key) override {
        std::string query = "DELETE FROM Players WHERE id = " + std::to_string(key);
        return _db->Execute(query);
    }

private:
    std::shared_ptr<Database::ISqlDatabase> _db;
};

}
