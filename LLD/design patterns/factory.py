from enum import Enum
from abc import ABC

class NotificationType(Enum):
    EMAIL=0
    SMS=1
    WHATSAPP=2

class Notification(ABC):
    @staticmethod
    def send(self):
        pass

class EmailNotification(Notification):
    def __init__(self):
        pass
    def send(self):
        print("Email")

class SMSlNotification(Notification):
    def __init__(self):
        pass
    def send(self):
        print("SMS")

class WhatAppNotification(Notification):
    def __init__(self):
        pass
    def send(self):
        print("WhatsApp")

class NotificationCenter():
    def createNotification(self, notificatinType:NotificationType):

        if notificatinType==NotificationType.EMAIL:
            return EmailNotification()

        elif notificatinType==NotificationType.WHATSAPP:
            return WhatAppNotification()

        if notificatinType==NotificationType.SMS:
            return SMSlNotification()

if __name__=="__main__":
    notification=NotificationCenter()
    email=notification.createNotification(NotificationType.EMAIL)
    sms=notification.createNotification(NotificationType.SMS)
    whatsapp=notification.createNotification(NotificationType.WHATSAPP)

    print(email.send(), sms.send(), whatsapp.send())