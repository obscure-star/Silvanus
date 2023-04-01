import boto3
import json
from botocore.exceptions import ClientError


def lambda_handler(event, context):
    # Connect to DynamoDB
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('Silvanus_sensors')

    # Initialize variables for pagination
    latest_record = None
    last_evaluated_key = None

    # Use pagination to iterate over all records in the table
    while True:
        # Get a batch of records from the table
        if last_evaluated_key:
            response = table.scan(ExclusiveStartKey=last_evaluated_key)
        else:
            response = table.scan()
        items = response.get('Items')

        # Iterate over the records in the batch and find the latest one
        for item in items:
            if latest_record is None or item['timestamp'] > latest_record['timestamp']:
                latest_record = item

        # Check if there are more records to fetch
        last_evaluated_key = response.get('LastEvaluatedKey')
        if not last_evaluated_key:
            break

    # Check if the data in the latest record is beyond the threshold
    if latest_record['alarm_mail'] == True:
        # Connect to SES (Simple Email Service)
        ses = boto3.client('ses')

        # Define the email message
        message = "The data in the latest record is an ALARM WARNING! Current timestamp: {}".format(
            latest_record['timestamp'])
        subject = "Threshold exceeded"
        recipient = "silvanusinnovate@gmail.com"
        sender = "silvanusinnovate@gmail.com"

        # Send the email
        try:
            ses.send_email(
                Destination={
                    'ToAddresses': [
                        recipient,
                    ],
                },
                Message={
                    'Subject': {
                        'Data': subject
                    },
                    'Body': {
                        'Text': {
                            'Data': message
                        }
                    }
                },
                Source=sender
            )
        except ClientError as e:
            print(e.response['Error']['Message'])
        else:
            print("Email sent successfully.")

    return {
        'statusCode': 200,
        'body': json.dumps('Success')
    }


"""import boto3
import json
from botocore.exceptions import ClientError

def lambda_handler(event, context):
    # Connect to DynamoDB
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('Silvanus_sensors')

    # Get all records from the table
    response = table.scan()
    items = response.get('Items')

    # Iterate over the records and find the latest one
    latest_record = None
    for item in items:
        if latest_record is None or item['timestamp'] > latest_record['timestamp']:
            latest_record = item

    # Check if the data in the latest record is beyond the threshold
    if latest_record['alarm_mail'] == True:
        # Connect to SES (Simple Email Service)
        ses = boto3.client('ses')

        # Define the email message
        message = "The data in the latest record is an ALARM WARNING! Current timestamp: {}".format(latest_record['timestamp'])
        subject = "Threshold exceeded"
        recipient = "silvanusinnovate@gmail.com"
        sender = "silvanusinnovate@gmail.com"

        # Send the email
        try:
            ses.send_email(
                Destination={
                    'ToAddresses': [
                        recipient,
                    ],
                },
                Message={
                    'Subject': {
                        'Data': subject
                    },
                    'Body': {
                        'Text': {
                            'Data': message
                        }
                    }
                },
                Source=sender
            )
        except ClientError as e:
            print(e.response['Error']['Message'])
        else:
            print("Email sent successfully.")

    return {
        'statusCode': 200,
        'body': json.dumps('Success')
    }
"""