class Communication {
	protected:
		int scktDesc;
		int port;
	public:
		~Communication();
		Communication();
		Communication(int scktDesc);
		int getSocketDesc();
};
