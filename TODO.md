## Priority fixes

- Wizards button hidden on windows (until the window is resized)
- forgot password button on server pages
- combobox with recent servers on server pages
- update daemon to not use project uuid
- update py api to not use current project
- add pixel aspect ratio in py api
- reimplement qmessagebox static methods as dumessagebox to improve display using DuUI::exec
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
- maya lock transform fails on publish
- special chars in maya

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
- removeProjects
- removeUsers
- setUserAssignments
- setEmail
- getEmail
- sync : username is not included, role can only be pulled but not pushed, email is ignored

## Daemon Doc

- remove serCurrentProject
