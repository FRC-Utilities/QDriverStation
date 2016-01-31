Before changing an existing translation, please do the following:

- Run the "lupdate" tool from terminal, this will "sync" the translations with the current code

If you want to create a new translation, please do the following:

- Open QDriverStation.pro with any text editor
- Go to the "Translations" section (at the bottom of the file)
- Add a new line with your language code:
    - For example, if you want to create a Russian translation, you would add this line:
    - > TRANSLATIONS += $$PWD/translations/qds_ru.ts
- Update the "translations.qrc" file (under the resources section)
- Do your translations and run the "lrelease" tool 

Once you finish creating or improving a translation, please do the following:

- Run the "lrelease" tool from terminal
- Recompile the QDriverStation