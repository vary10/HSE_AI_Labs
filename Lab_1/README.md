
Изменения относительно начальной версии:
- при выборе нового заказа выбирается заказ с наименьшим priority = (distance - 310) * timeLeft^2.
- если бот дальше, чем 300(см) от currentDestination, то в цикле проверяется расстояние до всех домов с заказами, и если до какого-то оно меньше 380, то бот бежит к нему. Это позволяет выполнять заказы по пути. После выполнения такого заказа бот возвращается к выполнению прежнего заказа в силу формулы priority.
![](https://github.com/vary10/HSE_AI_Labs/blob/master/Lab_1/Screen%20Shot%202016-10-04%20at%2012.39.13.png)
![](https://github.com/vary10/HSE_AI_Labs/blob/master/Lab_1/Screen%20Shot%202016-10-04%20at%2013.10.46.png)
![](https://github.com/vary10/HSE_AI_Labs/blob/master/Lab_1/Screen%20Shot%202016-10-04%20at%2013.24.57.png)
![](https://github.com/vary10/HSE_AI_Labs/blob/master/Lab_1/Screen%20Shot%202016-10-04%20at%2014.49.35.png)
