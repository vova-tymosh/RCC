# RCC

Remote Command and Control (RCC) is an easy-to-use and affordable wireless solution for controlling model railroads. It was started as a project to control outdoor locomotives of a G-scale, and now it can be used for smaller trains as well. It is completely open source - all software and hardware is available to study, modify and expand. And it is free (almost, you’d have to pay to manufacture hardware).

Details about locomotive decoders, throttles, turnouts and how to build them can be found on [RCC-Nodes](https://github.com/vova-tymosh/RCC-Nodes).


##Is RCC good for me?
If you are happy with your current setup the answer is obviously “no”. If you are tired of cleaning rails or you don’t want to spend an arm and a leg - RCC is your friend. Think about this. An RCC decoder costs under $30. It is tiny - an inch by two inches. It can drive a motor, play sound and control all blows and whistles your locomotive has. You can build your own Throttle or you may use your phone. After all, you just throw rails on the ground and run your trains! RCC works on the simplest scenarios with a single locomotive or it can be part of a complex layout (it is able to talk to JMRI).


##Does RCC use batteries?
Yes. In the 21st century some of the real trains are running on batteries. It’s time for the model ones to do the same. A battery that gives you 10h running time costs $30. Also, you may always swap for a fresh battery and keep running virtually indefinitely. With batteries you don’t need to worry about wiring, cleaning rails or transformers.
You hate batteries or you already have a wired setup. Ok, you may still use an RCC decoder, just power it from rail pickups via a rectifier.


##Why do we need yet another solution?
There are plenty of approaches/solutions available. Some are very affordable, like DCC-EX, but still require electrical conductivity between all the rail sections and locomotive wheels. Some are wireless, but for G-scale they cost $100-$200. RCC excel in both - it is dirt cheap and it doesn’t need wires. Also it is open source. You can change it to your unique needs and desires.


##Do I need a degree in electrical engineering to use RCC?
Fear not. There is nothing really complicated ahead of you. You’d need to do some mouse-clicking and do little soldering. Some folks are scared of this “soldering” part, but I’ll walk you through the process with videos and everything. After all, if you want a $30 decoder you have to do some work yourself. There is no magic in this world.
Here is a link to how to build a decoder - RCC-Nodes/Loco


##What is in this repository?
All the instructions and documentation about how to build stuff is located -  https://github.com/vova-tymosh/RCC-Nodes. If you are not interested in the internals of RCC you may go there right away. If you are interested or if you want to build the software yourself this repository is for you. It includes an Arduino library for all the communication protocols, controlling peripherals and other essential parts of the RCC solution.  


##What is the current status?
Currently RCC supports a couple variants of loco decoders. Complete solutions for throttle, turnouts and other things will come soon. Details about what kind of decoders are available and what they can do are provided on the same RCC-Nodes page.
