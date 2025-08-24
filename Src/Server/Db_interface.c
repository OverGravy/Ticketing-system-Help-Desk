#include "../../Libs/Server/Db_interface.h"

int init_db(sqlite3 **db, char *db_path)
{
    // Open the database file
    int rc = sqlite3_open(db_path, db);
    if (rc != SQLITE_OK)
    {
        return -1; // Failed to open database
    }

    // SQL to create the tickets table if it does not exist
    const char *sql_create_ticket =
        "CREATE TABLE IF NOT EXISTS tickets ("
        "id INTEGER PRIMARY KEY,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "date TEXT,"
        "priority INTEGER,"
        "status INTEGER,"
        "client_id INTEGER);";

    // SQL to drop the agents table if it exists
    const char *sql_drop_agent =
        "DROP TABLE IF EXISTS agents;";

    // SQL to create the agents table
    const char *sql_create_agent =
        "CREATE TABLE agents ("
        "id INTEGER PRIMARY KEY,"
        "password INTEGER);";

    char *errMsg = NULL;

    // Execute tickets table creation
    rc = sqlite3_exec(*db, sql_create_ticket, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(errMsg);
        return rc; // Error creating tickets table
    }

    // Execute agents table drop
    rc = sqlite3_exec(*db, sql_drop_agent, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(errMsg);
        return rc; // Error dropping agents table
    }

    // Execute agents table creation
    rc = sqlite3_exec(*db, sql_create_agent, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(errMsg);
        return rc; // Error creating agents table
    }

    // If all operations succeeded, return success code
    return rc;
}

int add_ticket(sqlite3 *db, const Ticket *ticket)
{

    // prepare the insert string
    const char *sql_insert =
        "INSERT INTO tickets (title, description, date, priority, status, client_id) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        return -1; // Failed to prepare statement
    }

    // adding the fields to populate the insert
    sqlite3_bind_text(stmt, 1, ticket->title, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, ticket->description, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, ticket->date, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, ticket->priority);
    sqlite3_bind_int(stmt, 5, ticket->status);
    sqlite3_bind_int(stmt, 6, ticket->client_id);

    // execute the query
    rc = sqlite3_step(stmt);

    // free the stmt
    sqlite3_finalize(stmt);

    // see the result
    if (rc != SQLITE_DONE)
    {
        return -1; // errore durante l'esecuzione
    }

    return 0; // successo
}

int add_agent(sqlite3 *db, int agent_id, int pwd)
{
    const char *sql = "INSERT INTO agents (id, password) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    int rc;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        return -1; // Failed to prepare statement
    }

    // Bind parameters
    rc = sqlite3_bind_int(stmt, 1, agent_id); // Bind agent_id to first '?'
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return -1; // Failed to bind parameter
    }

    rc = sqlite3_bind_int(stmt, 2, pwd); // Bind password to second '?'
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return -1; // Failed to bind parameter
    }

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        if (rc == SQLITE_CONSTRAINT)
        {
            sqlite3_finalize(stmt);
            return 1; // Agent with that id already exists
        }
        sqlite3_finalize(stmt);
        return -1; // Some other error occurred
    }

    // Clean up
    sqlite3_finalize(stmt);
    return 0; // Success
}

int get_agent_password(sqlite3 *db, int agent_id, int *pwd_out)
{
    const char *sql = "SELECT password FROM agents WHERE id = ?;";
    sqlite3_stmt *stmt;
    int rc;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        return -1; // Failed to prepare statement
    }

    // Bind agent_id parameter
    rc = sqlite3_bind_int(stmt, 1, agent_id);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return -1; // Failed to bind parameter
    }

    // Execute the query
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        // Retrieve the password from the result
        *pwd_out = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return 0; // Success
    }
    else if (rc == SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return 1; // No agent found with that id
    }
    else
    {
        sqlite3_finalize(stmt);
        return -1; // Some database error
    }
}

int get_ticket(sqlite3 *db, const TicketQuery *query, TicketPile *pile)
{
    char sql[512] = "SELECT id, title, description, date, priority, status, client_id FROM tickets WHERE 1=1";

    Ticket current_ticket; // Temporary ticket to hold each fetched ticket

    // Add conditions dynamically
    if (query->ticket_id != INT_UNUSED)
        strncat(sql, " AND id = ?", sizeof(sql) - strlen(sql) - 1);
    if (strcmp(query->title, STR_UNUSED) != 0)
        strncat(sql, " AND title LIKE ?", sizeof(sql) - strlen(sql) - 1);
    if (strcmp(query->description, STR_UNUSED) != 0)
        strncat(sql, " AND description LIKE ?", sizeof(sql) - strlen(sql) - 1);
    if (query->client_id != INT_UNUSED)
        strncat(sql, " AND client_id = ?", sizeof(sql) - strlen(sql) - 1);

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        return -1; // Error preparing statement
    }

    int idx = 1;
    if (query->ticket_id != INT_UNUSED)
    {
        sqlite3_bind_int(stmt, idx++, query->ticket_id);
    }
    if (strcmp(query->title, STR_UNUSED) != 0)
    {
        char title_like[256];
        snprintf(title_like, sizeof(title_like), "%%%s%%", query->title);
        sqlite3_bind_text(stmt, idx++, title_like, -1, SQLITE_TRANSIENT);
    }
    if (strcmp(query->description, STR_UNUSED) != 0)
    {
        char desc_like[256];
        snprintf(desc_like, sizeof(desc_like), "%%%.*s%%",
                 (int)(sizeof(desc_like) - 3),
                 query->description);
        sqlite3_bind_text(stmt, idx++, desc_like, -1, SQLITE_TRANSIENT);
    }
    if (query->client_id != INT_UNUSED)
    {
        sqlite3_bind_int(stmt, idx++, query->client_id);
    }

    // Loop su tutte le righe restituite
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {

        current_ticket.id = sqlite3_column_int(stmt, 0);
        strncpy(current_ticket.title, (const char *)sqlite3_column_text(stmt, 1), MAX_TITLE_LEN);
        strncpy(current_ticket.description, (const char *)sqlite3_column_text(stmt, 2), MAX_DESC_LEN);
        strncpy(current_ticket.date, (const char *)sqlite3_column_text(stmt, 3), 11);
        current_ticket.priority = sqlite3_column_int(stmt, 4);
        current_ticket.status = (TicketStatus)sqlite3_column_int(stmt, 5);
        current_ticket.client_id = sqlite3_column_int(stmt, 6);

        if (push_new_ticket(pile, &current_ticket) != 0)
        {
            sqlite3_finalize(stmt);
            return -1; // Memory allocation error
        }
    }

    if (rc == SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return pile->count; // Numero di ticket trovati
    }
    else
    {
        sqlite3_finalize(stmt);
        return -1; // Errore SQLite
    }
}

int get_ticket_and_mod(sqlite3 *db, const TicketQueryAndMod *query_and_mod)
{
    char sql[2048];
    char where_clause[512] = "";
    char set_clause[512] = "";
    int first = 1;
    sqlite3_stmt *stmt;
    int rc;

    // ---- Build WHERE clause for filters ----
    if (query_and_mod->filters.ticket_id != INT_UNUSED)
    {
        strcat(where_clause, first ? "WHERE " : " AND ");
        strcat(where_clause, "id = ?");
        first = 0;
    }
    if (strcmp(query_and_mod->filters.title, STR_UNUSED) != 0)
    {
        strcat(where_clause, first ? "WHERE " : " AND ");
        strcat(where_clause, "title = ?");
        first = 0;
    }
    if (strcmp(query_and_mod->filters.description, STR_UNUSED) != 0)
    {
        strcat(where_clause, first ? "WHERE " : " AND ");
        strcat(where_clause, "description = ?");
        first = 0;
    }
    if (query_and_mod->filters.client_id != INT_UNUSED)
    {
        strcat(where_clause, first ? "WHERE " : " AND ");
        strcat(where_clause, "client_id = ?");
        first = 0;
    }

    // ---- Build SET clause for updates ----
    int first_set = 1;

    if (query_and_mod->mod.new_client_id != INT_UNUSED)
    {
        strcat(set_clause, first_set ? "" : ", ");
        strcat(set_clause, "client_id = ?");
        first_set = 0;
    }
    if (query_and_mod->mod.new_priority != INT_UNUSED)
    {
        strcat(set_clause, first_set ? "" : ", ");
        strcat(set_clause, "priority = ?");
        first_set = 0;
    }
    if (query_and_mod->mod.new_status != (TicketStatus)INT_UNUSED)
    {
        strcat(set_clause, first_set ? "" : ", ");
        strcat(set_clause, "status = ?");
        first_set = 0;
    }

    // ---- If no SET fields, return error ----
    if (strlen(set_clause) == 0)
    {
        return -1; // nothing to update
    }

    // ---- First: COUNT rows matching WHERE ----
    snprintf(sql, sizeof(sql), "SELECT COUNT(*) FROM tickets %s;", where_clause);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
        return rc;

    int bind_index = 1;

    // Bind WHERE params
    if (query_and_mod->filters.ticket_id != INT_UNUSED)
        sqlite3_bind_int(stmt, bind_index++, query_and_mod->filters.ticket_id);
    if (strcmp(query_and_mod->filters.title, STR_UNUSED) != 0)
        sqlite3_bind_text(stmt, bind_index++, query_and_mod->filters.title, -1, SQLITE_STATIC);
    if (strcmp(query_and_mod->filters.description, STR_UNUSED) != 0)
        sqlite3_bind_text(stmt, bind_index++, query_and_mod->filters.description, -1, SQLITE_STATIC);
    if (query_and_mod->filters.client_id != INT_UNUSED)
        sqlite3_bind_int(stmt, bind_index++, query_and_mod->filters.client_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return rc;
    }

    int row_count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (row_count == 0)
    {
        return 2; // no matching ticket
    }
    else if (row_count > 1)
    {
        return 1; // multiple rows would be affected -> abort
    }

    // ---- If exactly one row, proceed with UPDATE ----
    snprintf(sql, sizeof(sql), "UPDATE tickets SET %s %s;", set_clause, where_clause);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
        return rc;

    bind_index = 1;

    // Bind SET params first
    if (query_and_mod->mod.new_client_id != INT_UNUSED)
        sqlite3_bind_int(stmt, bind_index++, query_and_mod->mod.new_client_id);
    if (query_and_mod->mod.new_priority != INT_UNUSED)
        sqlite3_bind_int(stmt, bind_index++, query_and_mod->mod.new_priority);
    if (query_and_mod->mod.new_status != (TicketStatus)INT_UNUSED)
        sqlite3_bind_int(stmt, bind_index++, query_and_mod->mod.new_status);

    // Bind WHERE params again
    if (query_and_mod->filters.ticket_id != INT_UNUSED)
        sqlite3_bind_int(stmt, bind_index++, query_and_mod->filters.ticket_id);
    if (strcmp(query_and_mod->filters.title, STR_UNUSED) != 0)
        sqlite3_bind_text(stmt, bind_index++, query_and_mod->filters.title, -1, SQLITE_STATIC);
    if (strcmp(query_and_mod->filters.description, STR_UNUSED) != 0)
        sqlite3_bind_text(stmt, bind_index++, query_and_mod->filters.description, -1, SQLITE_STATIC);
    if (query_and_mod->filters.client_id != INT_UNUSED)
        sqlite3_bind_int(stmt, bind_index++, query_and_mod->filters.client_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK; // success
}

int close_database(sqlite3 *db)
{
    if (db == NULL)
    {
        return -1; // Database wasn't opend
    }

    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK)
    {
        return rc; // error while closing the database
    }

    return SQLITE_OK; // closure happend succesfully
}