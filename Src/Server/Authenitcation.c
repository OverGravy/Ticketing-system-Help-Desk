#include "../../Libs/Server/Authenitcation.h"

int Singing_in(sqlite3 *db, int id)
{
    // generate a key for the user
    srand(time(NULL));
    int pwd = id * 12345 + (rand() % 2000 + 1); // even if the id is 0 you have a number that operate as a key

    // check if the agent already exists in the database
    int existing_pwd;
    int rc = get_agent_password(db, id, &existing_pwd);
    if (rc == SQLITE_OK)
    {
        // agent already exists
        return 0;
    }
    else if (rc != SQLITE_NOTFOUND)
    {
        // some database error
        return -1;
    }

    // add the new agent to the database
    rc = add_agent(db, id, pwd);
    if (rc != SQLITE_OK)
    {
        return -1; // error inserting
    }

    return pwd; // success
}

// Function to handle user login checking if the agent exists in the list
int Logging_in(sqlite3 *db, int id, int pwd)
{
    int table_pwd;
    int rc = get_agent_password(db, id, &table_pwd);

    // Check if query went well
    if (rc == SQLITE_OK)
    {
        // Agent exists, check password
        if (table_pwd == pwd)
        {
            return 0; // Login success
        }
        else
        {
            return 1; // Wrong password
        }
    }
    else if (rc == SQLITE_NOTFOUND)
    {
        // Agent not found
        return -1;
    }
    else
    {
        // Some database error
        return -2;
    }
}