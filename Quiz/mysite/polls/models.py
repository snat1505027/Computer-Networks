# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models
from django.utils.timezone import now

# Create your models here.
class Question(models.Model):
    question_text = models.CharField(max_length=200)
    pub_date = models.DateTimeField('date published')
    def __str__(self):
        return self.question_text


class Choice(models.Model):
    question = models.ForeignKey(Question, on_delete=models.CASCADE)
    choice_text = models.CharField(max_length=200)
    votes = models.IntegerField(default=0)
    def __str__(self):
        return self.choice_text

class UserPass(models.Model):
	userid = models.CharField(max_length=20)
	password = models.CharField(max_length=20)
	def __str__(self):
		return (self.userid+self.password)
		

class AccountTable(models.Model):
	account_type = models.CharField(max_length=50)
	user_name = models.CharField(max_length=20)
	password = models.CharField(max_length=20)
	age = models.IntegerField(default=0)
	sex = models.CharField(max_length=10)
	marital_status = models.CharField(max_length=10)
	nominee_id = models.ForeignKey("self", on_delete=models.CASCADE, null=True)
	balance = models.IntegerField(default=0)
	def __str__(self):
		return (self.account_type + '\n' + self.user_name + '\n' + self.password + '\n' + str(self.age) + '\n' + self.sex + '\n' + self.marital_status + '\n' + str(self.balance) + '\n')

class CarLoanData(models.Model):
	account_id = models.ForeignKey(AccountTable, on_delete=models.CASCADE, related_name='acc_id')
	monthly_income = models.IntegerField(default=0)
	monthly_expenditure = models.IntegerField(default=0)
	total_asset = models.IntegerField(default=0)
	car_price = models.IntegerField(default=0)
	requested_loan_amount = models.IntegerField(default=0)
	car_condition = models.CharField(max_length=20)
	car_vendor = models.CharField(max_length=20)
	car_brand = models.CharField(max_length=20)
	nominee_id = models.ForeignKey(AccountTable, on_delete=models.CASCADE, related_name='nom_id')	
	apply_date = models.DateTimeField(default=now, null=True)
	def __str__(self):
		return ( str(self.account_id) + '\nOwner\n' + 
			str(self.monthly_income) + '\n' + 
			str(self.monthly_expenditure) + '\n' +
			str(self.total_asset) + '\n' +
			str(self.car_price) + '\n' +  
			str(self.requested_loan_amount) + '\n' +
			self.car_condition + '\n' +
			self.car_vendor + '\n' +
			self.car_brand + '\nNominee:\n' +
		 	str(self.nominee_id) + '\n' +
		 	str(self.apply_date) + '\n')
#cc = CarLoanData(account_id=1, monthly_income = 20000, monthly_expenditure = 5000, total_asset = 100000, car_price = 800000, requested_loan_amount = 500000, car_condition = "old", car_vendor = "type1", car_brand="toyota")
#from polls.models import AccountTable, CarLoanData