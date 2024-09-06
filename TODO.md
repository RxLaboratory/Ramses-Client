## UI

- Use DuUI Layout methods everywhere
- DuDockWidget update
- QDateEdit button CSS
- Fix title bars
- Update all the rest

## Project Wizard

- Shots
- Assets

## New server interface

- CLient side
    - WIP reimplement API (add project methods, see python client)
    - refactor logging & console (see DuME)
- check how serveruuid is used and remove if not needed anymore

## Landing

- Local DB: store the project uuid nest to the user uuid and automatically set it
- server pull and push refuse RamProjet except current

- Create Team Project
    - FIX: project/user not set after creation?
    - (Improve server widget (hide details in a QMenu), keep only the address)
    - Setup project (add page to create/assign users)
    - Create users on server
    - Assign users on server
- Join Team Project
    - Login on server
    - List available projects
    - Create DB (cf former db creator online)
    - Sync

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

## Server Doc

- createUsers
- createProject
- assignUsers
- unassignUsers
- setUserRole
- setEmail
- getEmail
- sync : username is not included, role can only be pulled but not pushed, email is ignored
