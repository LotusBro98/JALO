from django.db import models

# Create your models here.

class TargetData(models.Model):
    frame = models.IntegerField()
    time = models.DateTimeField()
    person_id = models.IntegerField(blank=True, null=True)
    target = models.TextField(blank=True, null=True)
    pos_x = models.FloatField()
    pos_y = models.FloatField()
    dir_x = models.FloatField(blank=True, null=True)
    dir_y = models.FloatField(blank=True, null=True)

    class Meta:
        managed = False
        db_table = 'target_data'
