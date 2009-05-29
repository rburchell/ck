#!/usr/bin/python
"""A more complex provider with a horroristic theme."""

from ContextKit.flexiprovider import *
import gobject

count = 0
def there_are_more():
    global count
    count += 1
    fp.set('Zombie.Count', count)
    if count == 2:
        fp.add_and_update(STRING('You.Shout'))
        fp.set('You.Shout', 'omg zombies!')
    if count == 5:
        fp.set('Life.isInDanger', True)
    return True

gobject.timeout_add_seconds(2, there_are_more)
fp = Flexiprovider([INT('Zombie.Count'), TRUTH('Life.isInDanger')],
                   'omg.zombies', 'session')
fp.run()
