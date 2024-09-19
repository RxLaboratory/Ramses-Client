## Priority fixes

- Add a server admin mode, from landing page
    - manage all projects (create, disable, remove)
    - manage all users (create, disable, remove)
    - manage user assignments (per user or per project)
- show online/offline/security/not team
    - reimplement sync buttons and setonline/setoffline
- offline mode if network unavailable for team projects (skip login, add a button to skip/set offline on login dialog)
- update daemon to not use project uuid
- update py api to not use current project
- add pixel aspect ratio in py api
- auto show console on qcritical
- reimplement qmessagebox static methods as dumessagebox to improve display using DuUI::exec
- do the same as above with qdialog/dudialog
- Project wizard: step templates, shots, assets
- objectlistwidget: add a show details button

## Other

- Refactor folders: all from project (done ? check)

## Client UI

- Use DuUI Layout methods everywhere
- QDateEdit button CSS
- don't user titlebars where not needed; remove duqftitlebar and use simple titlebar; remove closerequested and reinit
- reimplement all object editors
- Idée : verrouiller le pipe
- schedule : changer les couleurs de plusieurs cases à la fois
- schedule : déplacements des blocs, en drag'n'drop plutot que copier coller si possible (avec Shift?)
- schedule : mettre une "frontière" sous certaines lignes pour que ça reste toujours en haut, comme dans excel (quand on a bcp de lignes et qu'on est en bas, on voit plus les deadlines tout en haut)
- schedule : qu'on puisse zoomer/dézoomer le planning
- item tables : freeze first column or create header delegate

## Maya

- assets pas filtrés par group ?

## Project Wizard

- Sequences and Shots
- Asset groups and Assets

## New server interface

- Remove all userChanged (the user can't change), but use databaseReady when needed
- Implement a Ramses::roleChanged signal, and use everywhere in UI (emit just after ready)
- Make sure on sync that the user role is monitored
- update settings system: remove per user, per project settings etc
- update pipeline: sync node location
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
