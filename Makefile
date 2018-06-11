default:
	@gcc -o timerfd_demo  timerfd_demo.c
clean:
	@rm -rf timerfd_demo