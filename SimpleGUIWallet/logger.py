##
##

class LoggerInstance:
    message_window = None
    message_body = ""
    mirror_to_stdout = False

    def Write(self, msgtext, *, echo=None):
        if echo == True or (self.mirror_to_stdout==True and echo!=False):
            print(msgtext)
        self.message_body += msgtext + "\n"
        self.message_window.configure(text=self.message_body)
        self.message_window.update()

    def Clear(self):
        self.message_body = ""
        self.message_window.configure(text=self.message_body)
        self.message_window.update()

    def SetMessageWidget(self, msgbox):
        self.message_window = msgbox

Logger = LoggerInstance()
