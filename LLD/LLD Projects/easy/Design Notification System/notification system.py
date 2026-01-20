from enum import Enum
from abc import ABC
class NotificationType(Enum):
     EMAIL=0
     SMS=1
     PUSH=2
class Recipient:
    def __init__(self,userId, email, ph):
        self.userId=userId
        self.email=email
        self.ph=ph

class Notification(ABC):
    def __init__(self, id, message, notification_type):
        self.id=id
        self.message=message
        self.notificationType=notification_type


class NotificationService():
    def __init__(self):
        pass

    def sendNotification(self, notification):
        pass

class EmailGateway():
    pass
class SMSGateway():
    pass
class PushGateway():
    pass
class NotificationSystemDemo():
    def demo(self):
        # create notification
        recipient1=Recipient("123","abc@xyz.com", "991")
        recipient2=Recipient("123","abc@xyz.com", "992")

        email_notification=Notification('11', "Hi", NotificationType.EMAIL)
        sms_notification=Notification('12', "Hello", NotificationType.SMS)
        push_notification=Notification('13', "Holla", NotificationType.PUSH)

        notificationService=NotificationService()

        notificationService.sendNotification(email_notification,recipient1)
        notificationService.sendNotification(sms_notification,recipient2)
        notificationService.sendNotification(push_notification,recipient1)


if __name__=="__main__":
    NotificationSystemDemo.demo()