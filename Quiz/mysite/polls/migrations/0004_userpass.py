# Generated by Django 2.1.5 on 2019-02-12 15:24

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('polls', '0003_carloandata_apply_date'),
    ]

    operations = [
        migrations.CreateModel(
            name='UserPass',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('userid', models.CharField(max_length=20)),
                ('password', models.CharField(max_length=20)),
            ],
        ),
    ]