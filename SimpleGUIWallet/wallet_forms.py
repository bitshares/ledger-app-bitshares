import tkinter as tk
import Logger

class TransferOpFrame(tk.Frame):

    def __init__(self, parent, *args, **kwargs):

        self.send_command = kwargs.pop('command', lambda *args, **kwargs: None)
        if not 'background' in kwargs: kwargs["background"] = parent["background"]

        tk.Frame.__init__(self, parent, *args, **kwargs)

        common_args={}
        common_args["background"] = self["background"]

        ##
        ## Upper Spacer:
        ##

        lblSpacerActiveTop = tk.Label(self, text="", **common_args)
        lblSpacerActiveTop.pack(expand=True, fill="y")

        ##
        ## Destination Account:
        ##

        frameToWhom = tk.Frame(self, **common_args)
        frameToWhom.pack(padx=10, pady=5, fill="x")

        self.to_account_name = tk.Entry(frameToWhom)
        self.to_account_name.pack(side="right", padx=10)

        labelSendTo = tk.Label(frameToWhom, text="Send To: (BitShares User Account)",
                               font=("Helvetica", 16), **common_args)
        labelSendTo.pack(side="right")

        ##
        ## Amount and Asset:
        ##

        frameSendAmount = tk.Frame(self, **common_args)
        frameSendAmount.pack(padx=10, pady=5, fill="x")

        self.box_asset_to_send = tk.Entry(frameSendAmount, width=10)
        self.box_asset_to_send.pack(side="right", padx=10)

        labelAsset = tk.Label(frameSendAmount, text="Asset:",
                           font=("Helvetica", 16), **common_args)
        labelAsset.pack(padx=(20,0),side="right")

        self.box_amount_to_send = tk.Entry(frameSendAmount)
        self.box_amount_to_send.pack(side="right", padx=10)

        labelAmount = tk.Label(frameSendAmount, text="Amount:",
                            font=("Helvetica", 16), **common_args)
        labelAmount.pack(side="right")

        ##
        ## The Send Button:
        ##
        self.button_send = tk.Button(self, text="Send Transfer",
                                     command=lambda: self.button_send_handler()
        )
        self.button_send.pack(pady=30)

        ##
        ## Lower Spacer:
        ##

        lblSpacerActiveBottom = tk.Label(self, text="", **common_args)
        lblSpacerActiveBottom.pack(expand=True, fill="y")

    def button_send_handler(self):
        self.button_send.configure(state="disabled")
        Logger.Clear()
        try:
            if len(self.to_account_name.get()) == 0:
                Logger.Write("Please provide an account name to send to!")
                return
            if len(self.box_asset_to_send.get()) == 0:
                Logger.Write("Please specify asset to send!")
                return
            if len(self.box_amount_to_send.get()) == 0:
                Logger.Write("Please specify amount to send!")
                return
            self.send_command("Arg to Send Commend")
        finally:
            self.button_send.update() # Eat any clicks that occured while disabled
            self.button_send.configure(state="normal") # Return to enabled state
            Logger.Write("READY.")

        Logger.Write("Oooh")


class ActivityMessageFrame(tk.Frame):
    def __init__(self, parent, *args, **kwargs):

        if not 'background' in kwargs: kwargs["background"] = parent["background"]

        tk.Frame.__init__(self, parent, *args, **kwargs)

        common_args={}
        common_args["background"] = self["background"]

        log_frame = tk.LabelFrame(self, text="Activity", relief="groove",
                                  **common_args)
        log_frame.pack(expand=True, fill="both")
        self.messages = tk.Message(log_frame, text="",
                                   width=580, background="light gray",
                                   anchor="n", pady=8, font="fixed")
        self.messages.pack(expand=True, fill="both")
