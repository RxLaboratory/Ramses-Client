## Priority fixes

- Refactor project edit widget
    - and user assignments, for admin only
- to fix loading order: try to keep the uuid list in the model to reload the objects later (maybe monitor the corresponding dbtable to auto reload)
- WIP Edit Widget
    - toJson loadJson for RamUser
    - show estim for steps
    - open folder for users
- Assigned users not listed in projects on first open (must be an issue with the order of pulling tables) (try keeping the uuids to be reoladed later)
- show states (Connect states to indicators in mainwindow (is team, server status & ssl, state manager)
    - implement a state indicator / progress
    - add a progress bar to home page
- offline mode if network unavailable for team projects (skip login, add a button to skip/set offline on login dialog)
- Update/refactor progressmanager (with state manager) and homepage
- assign users in projecteditwidget (disabled for non admins)
- User edit widget: check if the role and email has changed to not update it if not necessary
- why so many status on pull? (and in the table! looks like they're removed instead of being modified)
- pipeline : crash on layout all steps
- update daemon to not use project uuid
- update py api to not use current project
- auto show console on qcritical
- user DuUI layout methods
- DuDockWidget update (and reenable Q_ASSERT in their event filter)
- QDateEdit css
- reimplement qmessagebox static methods as dumessagebox to improve display using DuUI::exec
- do the same as above with qdialog/dudialog
- Project wizard: step templates, shots, assets

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
- Idée : verrouiller le pipe
- schedule : changer les couleurs de plusieurs cases à la fois
- schedule : déplacements des blocs, en drag'n'drop plutot que copier coller si possible (avec Shift?)
- schedule : mettre une "frontière" sous certaines lignes pour que ça reste toujours en haut, comme dans excel (quand on a bcp de lignes et qu'on est en bas, on voit plus les deadlines tout en haut)
- schedule : qu'on puisse zoomer/dézoomer le planning
- item tables : freeze first column or update header 

## Maya

- assets pas filtrés par group
- update to PySide6 (replace PySide2 import, QAction is in QtGui, form layout spacing)

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
