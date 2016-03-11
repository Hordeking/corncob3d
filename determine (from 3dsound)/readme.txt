Dave,
  I did quite a bit of experimenting tonight into the problem. 
I use a Spontaneous Assembly routine to interrogate the timer chip
to make sure it got reset to the proper value.  It's called

get_timer0

  Anyway, I couldn't figure out why POWER runs great under Windows, 
but Corncob didn't.  After some elimination of timer code from Corncob,
I narrowed it down to the above mentioned get_timer0 call.  Apparently
it causes a system panic under Windows.  I think that there is a good
chance that the problems will go away when I simply remove the timer-reset
checks.
  I should have figured that if their set_timer0 function wasn't reliable,
that their get_timer0 function might have problems too.
  Anyway, let the secratary give this one a try.


  Corncob now runs great under windows for my system now, except for
the most annoying thing.  Sometimes after a mission is completed, Windows
plops a big ugly text box over my happy-landings screen.  The text tells
me that it's OK for me to use my POP-UP program now, and to hit cntrl-c
when I'm done to return to windows.  Then the only key that functions is
cntl-c, and your mission is NOT recorded.
  I have no way of knowing what triggers Windows to interrupt my program
with that stupid message.

                                                    -Kevin

