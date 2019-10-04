# Generated by Django 2.2.6 on 2019-10-04 09:51

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('jalo_display', '0002_auto_20191004_0937'),
    ]

    operations = [
        migrations.CreateModel(
            name='TargetData',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('frame', models.IntegerField()),
                ('time', models.DateTimeField()),
                ('person_id', models.IntegerField(blank=True, null=True)),
                ('target', models.TextField(blank=True, null=True)),
                ('pos_x', models.FloatField()),
                ('pos_y', models.FloatField()),
                ('dir_x', models.FloatField(blank=True, null=True)),
                ('dir_y', models.FloatField(blank=True, null=True)),
            ],
            options={
                'db_table': 'target_data',
                'managed': False,
            },
        ),
        migrations.DeleteModel(
            name='person_entry',
        ),
    ]
