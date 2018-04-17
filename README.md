# canbus-example
For improved accuracy this example uses the MCO output from the st-link instead of the HSI (internal) clock.
This requires modification to your nucleo32 board, namely: soldering across SB4. See page 19 of the [nucleo32 manual](http://www.st.com/content/ccc/resource/technical/document/user_manual/e3/0e/88/05/e8/74/43/a0/DM00231744.pdf/files/DM00231744.pdf/jcr:content/translations/en.DM00231744.pdf).

To initialise a project just like this one
- Open CubeMX
- Load project
- Select the file canbus-example.ioc from this project
- Change project name/directory in preferences
- Generate
