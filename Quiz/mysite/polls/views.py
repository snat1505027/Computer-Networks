# -*- coding: utf-8 -*-
from __future__ import unicode_literals
# Create your views here.
from django.http import HttpResponse
from django.template import loader

from .models import Question

from django.http import HttpResponse, HttpResponseRedirect
from django.shortcuts import render
from django.urls import reverse

from django.shortcuts import get_object_or_404, render

from .models import Choice, Question


'''
def index(request):
    return HttpResponse("Hello, world. You're at the polls index.")'''

def test(request):
	'''username_ = request.POST['username']
	print(username_)
	password_ = request.POST['password']
	print(password_)
	from polls.models import UserPass
	ent = UserPass(userid=username_, password = password_)
	ent.save()
	UserPass.objects.all()'''
	name_ = request.POST['name']
	acc_id_= request.POST['acc_id']
	total_asset_= request.POST['total_asset']
	monthly_income_= request.POST['monthly_income']
	monthly_expense_= request.POST['monthly_expense']
	car_price_= request.POST['car_price']
	loan_amt_= request.POST['loan_amt']
	car_brand_ = request.POST['car_brand']
	car_vendor_ = request.POST['car_vendor']
	car_cond_ = request.POST['car_cond']
	nom_id_ = request.POST['nom_id']

	from polls.models import CarLoanData, AccountTable

	acc1 = AccountTable.objects.get(id=int(acc_id_))
	print(acc1)
	acc2 = AccountTable.objects.get(id=int(nom_id_))
	print(acc2)
	
	entry = CarLoanData(account_id = acc1 , total_asset = total_asset_, monthly_income = monthly_income_, monthly_expenditure = monthly_expense_, car_price = car_price_, requested_loan_amount = loan_amt_, car_brand = car_brand_ , car_vendor = car_vendor_ , car_condition = car_cond_ , nominee_id = acc2)
	print(entry)

	entry.save()
	
	print(entry)

	return render(request,'success.html',context=None)
'''
def index(request):
    return render(request, 'index.html' ,context=None)
'''
def index(request):
    latest_question_list = Question.objects.order_by('-pub_date')[:5]
    template = loader.get_template('index.html')
    context = {
        'latest_question_list': latest_question_list,
    }
    return HttpResponse(template.render(context, request))

#def detail(request, question_id):
 #   return HttpResponse("You're looking at question %s." % question_id)

def results(request, question_id):
    response = "You're looking at the results of question %s."
    return HttpResponse(response % question_id)

#def vote(request, question_id):
 #   return HttpResponse("You're voting on question %s." % question_id)

'''
def detail(request, question_id):
    try:
        question = get_object_or_404(Question, pk=question_id)
    except Question.DoesNotExist:
        raise Http404("Question does not exist")

	return render(request, 'detail.html', {'question': question})
'''	
def detail(request, question_id):
    question = get_object_or_404(Question, pk=question_id)
    return render(request, 'detail.html', {'question': question})
	
def vote(request, question_id):
    question = get_object_or_404(Question, pk=question_id)
    try:
        selected_choice = question.choice_set.get(pk=request.POST['choice'])
    except (KeyError, Choice.DoesNotExist):
        # Redisplay the question voting form.
        return render(request, 'detail.html', {
            'question': question,
            'error_message': "You didn't select a choice.",
        })
    else:
        selected_choice.votes += 1
        selected_choice.save()
        # Always return an HttpResponseRedirect after successfully dealing
        # with POST data. This prevents data from being posted twice if a
        # user hits the Back button.
        return HttpResponseRedirect(reverse('results', args=(question.id,)))
# Leave the rest of the views (detail, results, vote) unchanged


def results(request, question_id):
    question = get_object_or_404(Question, pk=question_id)
    return render(request, 'results.html', {'question': question})

def login(request):
	return render(request,'login.html',context=None)