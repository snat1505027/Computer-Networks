# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.contrib import admin

# Register your models here.
from django.contrib import admin

from .models import Question, AccountTable, CarLoanData, UserPass

admin.site.register(Question)
admin.site.register(AccountTable)
admin.site.register(CarLoanData)
admin.site.register(UserPass)

