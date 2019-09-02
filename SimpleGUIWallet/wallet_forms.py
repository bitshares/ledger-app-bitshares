import tkinter as tk
import Logger

class WhoAmIFrame(tk.Frame):

    def __init__(self, parent, *args, **kwargs):

        self.button_command = kwargs.pop('command', lambda *args, **kwargs: None)
        self.textvariable = kwargs.pop('textvariable', None)
        if not 'background' in kwargs: kwargs["background"] = parent["background"]

        tk.Frame.__init__(self, parent, *args, **kwargs)

        common_args={}
        common_args["background"] = self["background"]

        lbl_from_account_name = tk.Label(self, text="BitShares User Account:",
                                         font=("Helvetica", 16), **common_args)
        lbl_from_account_name.pack(side="left")

        box_from_account_name = tk.Entry(self, width=30, textvariable=self.textvariable)
        box_from_account_name.pack(side="left", padx=10)

        self.button = tk.Button(self, text="Refresh Balances",
                                     command=lambda: self.button_handler())
        self.button.pack(side="left", padx=5)

    def button_handler(self):
        self.button.configure(state="disabled")
        Logger.Clear()
        try:
            account_name = self.textvariable.get()
            if len(account_name) == 0:
                Logger.Write("Please provide an account name!")
                return
            self.button_command(account_name)
        finally:
            self.button.update() # Eat any clicks that occured while disabled
            self.button.configure(state="normal") # Return to enabled state
            Logger.Write("READY.")


class AssetListFrame(tk.LabelFrame):

    def __init__(self, parent, *args, **kwargs):

        self.asset_text_var = kwargs.pop('assettextvariable', None)
        if not 'background' in kwargs: kwargs["background"] = parent["background"]

        tk.LabelFrame.__init__(self, parent, *args, **kwargs)

        common_args={}
        common_args["background"] = self["background"]

        self.Balances = ["One", "Two"]

        self.lst_assets = tk.Listbox(self, bd=0)
        self.lst_assets.pack(side="left", fill="y")
        self.lst_assets.bind("<ButtonRelease-1>", self.on_click)

        self.refresh()

    def setBalances(self, AssetList):
        self.Balances = AssetList
        self.refresh()

    def refresh(self):
        self.lst_assets.delete(0, tk.END)
        for item in self.Balances:
            self.lst_assets.insert(tk.END, str(item))

    def on_click(self, *args):
        try:
            idx = self.lst_assets.index(self.lst_assets.curselection())
            self.asset_text_var.set(self.Balances[idx].symbol)
        except:
            pass

class TransferOpFrame(tk.Frame):

    def __init__(self, parent, *args, **kwargs):

        self.send_command = kwargs.pop('command', lambda *args, **kwargs: None)
        self.asset_text_var = kwargs.pop('assettextvariable', None)
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

        self.box_asset_to_send = tk.Entry(frameSendAmount, width=10, textvariable=self.asset_text_var)
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
            account_name = self.to_account_name.get()
            asset_symbol = self.box_asset_to_send.get()
            amount = self.box_amount_to_send.get()
            if len(account_name) == 0:
                Logger.Write("Please provide an account name to send to!")
                return
            if len(asset_symbol) == 0:
                Logger.Write("Please specify asset to send!")
                return
            if len(amount) == 0:
                Logger.Write("Please specify amount to send!")
                return
            self.send_command(account_name, amount, asset_symbol)
        finally:
            self.button_send.update() # Eat any clicks that occured while disabled
            self.button_send.configure(state="normal") # Return to enabled state
            Logger.Write("READY.")


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
