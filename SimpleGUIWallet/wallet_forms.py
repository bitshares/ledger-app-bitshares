import tkinter as tk

class TransferOpFrame(tk.Frame):
    def __init__(self, parent, *args, **kwargs):

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

        to_account_name = tk.Entry(frameToWhom)
        to_account_name.pack(side="right", padx=10)

        labelSendTo = tk.Label(frameToWhom, text="Send To: (BitShares User Account)",
                               font=("Helvetica", 16), **common_args)
        labelSendTo.pack(side="right")

        ##
        ## Amount and Asset:
        ##

        frameSendAmount = tk.Frame(self, **common_args)
        frameSendAmount.pack(padx=10, pady=5, fill="x")

        box_asset_to_send = tk.Entry(frameSendAmount, width=10)
        box_asset_to_send.pack(side="right", padx=10)

        labelAsset = tk.Label(frameSendAmount, text="Asset:",
                           font=("Helvetica", 16), **common_args)
        labelAsset.pack(padx=(20,0),side="right")

        box_amount_to_send = tk.Entry(frameSendAmount)
        box_amount_to_send.pack(side="right", padx=10)

        labelAmount = tk.Label(frameSendAmount, text="Amount:",
                            font=("Helvetica", 16), **common_args)
        labelAmount.pack(side="right")

        ##
        ## The Send Button:
        ##
        button_send = tk.Button(self, text="Send Transfer",
                                #command=lambda: button_handler_Send(button_send, to_account_name)
        )
        button_send.pack(pady=30)

        ##
        ## Lower Spacer:
        ##

        lblSpacerActiveBottom = tk.Label(self, text="", **common_args)
        lblSpacerActiveBottom.pack(expand=True, fill="y")

