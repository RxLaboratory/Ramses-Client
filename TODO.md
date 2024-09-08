## Priority fixes

- Check users creation/assignment
- WIP Edit Widget
    - toJson loadJson for RamUser
    - show estim for steps
    - open folder for users
- Assigned users not listed in projects on first open (must be an issue with the order of pulling tables)
- show states (Connect states to indicators in mainwindow (is team, server status & ssl, state manager)
    - implement a state indicator / progress
    - add a progress bar to home page
- offline mode if network unavailable for team projects
- Update/refactor progressmanager (with state manager) and homepage
- assign users in projecteditwidget (disabled for non admins)

## Other

- Refactor folders: all from project
- reimplement save username and savepassword
- DBInterface::acceptClean -> replace old m_rsi method
- refactor setonline/setoffline (cf mainwindow)

## Client UI

- Use DuUI Layout methods everywhere
- DuDockWidget update
- QDateEdit button CSS
- Improve server widget (hide details in a QMenu), keep only the address
- refactor logging & console (see DuME)
- refactor userEditwidget
- message "not implemented" on forgot password (usereditwidget)
- don't user titlebars where not needed; remove duqftitlebar and use simple titlebar; remove closerequested and reinit
- reimplement all object editors

## Project Wizard

- Sequences and Shots
- Asset groups and Assets

## New server interface

- WIP reimplement All API
- Remove all userChanged (the user can't change), but use databaseReady when needed
- Implement a Ramses::roleChanged signal, and use everywhere in UI (emit just after ready)
- Make sure on sync that the user role is monitored
- update settings system: remove per user, per project settings etc
- update pipeline: sync node location
- Mainwindow etc: build UI only on ramsesReady signal (and disconnect everywhere else)
- remove serveruuid

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

- refactor admin page

### Global objects

Set per project:

- RamApplication
- RamFileType
- RamState

## Objects

- All objects: don't store project in data (not needed anymore)
- implement toJson and loadJson in RamObject derived classes
- reimplement object editors to use json
- Properties dock must delete its previous content; use qpointer in classes with property docks (search mw setPropertyDock method in code)

## Server changes

- setEmail verify if unique
- createUser(s), verify is email is unique
- reset email system

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
