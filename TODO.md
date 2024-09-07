## Landing

- Create Team Project
    - (Improve server widget (hide details in a QMenu), keep only the address)

## UI

- Use DuUI Layout methods everywhere
- DuDockWidget update
- QDateEdit button CSS
- Fix title bars
- Update all the rest
- Step Edit Widget broken
- Connect states to indicators in mainwindow (is team, server status & ssl, state manager)
- Update/refactor progressmanager (with state manager)

## Project Wizard

- Shots
- Assets

## New server interface

- CLient side
    - WIP reimplement API (add project methods, see python client)
    - refactor logging & console (see DuME)
    - Remove all userChanged (the user can't change), but use databaseReady when needed
    - Implement a Ramses::roleChanged signal, and use everywhere in UI (emit just after ready)
    - Make sure on sync that the user role is monitored
    - update settings system: remove per user, per project settings etc
    - update pipeline: sync node location
    - Mainwindow etc: build UI only on ramsesReady signal (and disconnect everywhere else)
- check how serveruuid is used and remove if not needed anymore


## Templates & Global & Object changes

### Implement a template system for all objects

offline à la DuME

- Use JSON files in a specific folder in the project ("_ramses_templates" ?)
- JSON object : objectType, data
- Extract internal templates in a "_internal" subfolder
- Templates with the same name and objectType as an internal take precedence
- Scan for all subfolders except (subfolder)
- Add a template list on top of the editor in a "create" mode

Remove RamTemplateStep and RamTemplateAssetGroup

### Global objects

Set per project:

- RamApplication
- RamFileType
- RamState

### Objects

- RamUser: don't store role or email or shortName in data, but use specific columns (update local data schema)
- All objects: don't store project in data (not needed anymore)

## Server changes

- setEmail verify if unique
- createUser(s), verify is email is unique

## Client

- remove databasecreatewidget and databaseeditwidget
- Refactor folders: all from project
- message "not implemented" on forgot password (usereditwidget)
- reimplement save username and savepassword
- DBInterface::acceptClean -> replace old m_rsi method
- refactor setonline/setoffline (cf mainwindow)
- refacotr quit/last sync, etc, using StateManager

## Server Doc

- createUsers
- createProject
- getAllUsers
- assignUsers
- unassignUsers
- setUserRole
- setEmail
- getEmail
- sync : username is not included, role can only be pulled but not pushed, email is ignored

## Daemon Doc

- remove serCurrentProject
