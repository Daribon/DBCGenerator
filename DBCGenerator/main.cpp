// This tool generates DBC and CSV files from database tables.
// Author: brotalnia
//

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <array>
#include <set>
#include <sstream>
#include <vector>

#include "Database\Database.h"
#include "Tables\AreaTrigger.h"
#include "Tables\Spell.h"
#include "Tables\SkillLineAbility.h"
#include "Tables\WorldSafeLocs.h"

Database GameDb;
const std::string CONNECTION_FILE = "config.ini";

std::string MakeConnectionString() {
    std::string mysql_host;
    std::string mysql_port;
    std::string mysql_user;
    std::string mysql_pass; // Password is not saved to file
    std::string mysql_db;

    std::ifstream inputFile(CONNECTION_FILE);

    // Attempt to load from file (excluding password)
    if (inputFile.is_open()) {
        if (std::getline(inputFile, mysql_host, ';') &&
            std::getline(inputFile, mysql_port, ';') &&
            std::getline(inputFile, mysql_user, ';') &&
            std::getline(inputFile, mysql_pass, ';') &&
            std::getline(inputFile, mysql_db, ';')) {
            inputFile.close();
            std::cout << "Loaded connection details from " << CONNECTION_FILE << std::endl;
        }
        else {
            std::cerr << "Error reading connection details from " << CONNECTION_FILE << std::endl;
            mysql_host.clear();
        }
    }

    // Prompt user if loading failed (or file doesn't exist)
    if (mysql_host.empty()) {
        std::cout << "Host: ";
        std::getline(std::cin, mysql_host);
        if (mysql_host.empty())
            mysql_host = "127.0.0.1";

        std::cout << "Port: ";
        std::getline(std::cin, mysql_port);
        if (mysql_port.empty())
            mysql_port = "3306";

        std::cout << "User: ";
        std::getline(std::cin, mysql_user);
        if (mysql_user.empty())
            mysql_user = "root";

        std::cout << "Password: ";
        std::getline(std::cin, mysql_pass);
        if (mysql_pass.empty())
            mysql_pass = "root";

        std::cout << "Database: ";
        std::getline(std::cin, mysql_db);
        if (mysql_db.empty())
            mysql_db = "mangos";

        // Save to file (excluding password)
        std::ofstream outputFile(CONNECTION_FILE);
        if (outputFile.is_open()) {
            outputFile << mysql_host << ";" << mysql_port << ";" << mysql_user << ";" <<  mysql_pass << ";" << mysql_db << ";";
            outputFile.close();
            std::cout << "Saved connection details to " << CONNECTION_FILE << std::endl;
        }
        else {
            std::cerr << "Error saving connection details to " << CONNECTION_FILE << std::endl;
        }
    }

    return mysql_host + ";" + mysql_port + ";" + mysql_user + ";" + mysql_pass + ";" + mysql_db;
}

int main()
{
    printf("\nEnter your database connection info.\n");
    std::string const connection_string = MakeConnectionString();

    printf("\nConnecting to database.\n");
    if (!GameDb.Initialize(connection_string.c_str()))
    {
        printf("\nError: Cannot connect to world database!\n");
        getchar();
        return 1;
    }

    printf("\nClient Build:\n");
    printf("> ");
    uint32 build = GetUInt32();

    printf("\nSelect table:\n");
    printf("1. spell_template\n");
    printf("2. skill_line_ability\n");
    printf("3. areatrigger_template\n");
    printf("4. world_safe_locs\n");
    printf("> ");
    uint32 option = GetUInt32();

    std::unique_ptr<DBCFile> dbc;
    switch (option)
    {
        case 1: // spell_template
        {
            dbc = std::make_unique<SpellDBC>();
            break;
        }
        case 2: // skill_line_ability
        {
            dbc = std::make_unique<SkillLineAbilityDBC>();
            break;
        }
        case 3: // areatrigger_template
        {
            dbc = std::make_unique<AreaTriggerDBC>();
            break;
        }
        case 4: // world_safe_locs
        {
            dbc = std::make_unique<WorldSafeLocsDBC>();
            break;
        }
        default:
        {
            printf("Wrong selection.\n");
            return 1;
        }
    }

    printf("Loading database...\n");
    dbc->LoadFromDB(build);

    printf("Exporting to dbc...\n");
    dbc->SaveToDBC();

    printf("Done.");
    getchar();
    
    GameDb.Uninitialise();
    return 0;
}

