#include "UsbWorker.h"

using namespace std;
using namespace chrono;

mutex UsbWorker::msendbuf;
mutex UsbWorker::mdevtrying;

const int UsbWorker::RECV_BUFFER_SIZE;
const int UsbWorker::SEND_BUFFER_SIZE;
const char UsbWorker::DEVICEID_S;
const char UsbWorker::DEVICEID_E;

UsbWorker::UsbWorker(int deviceId, int baud = 0, int deadAfterMs = 1500, function<void(char*)> callback_data = NULL, function<void(int)> callback_state = NULL)
{
	stop = false;

	_callback_data = callback_data;
	_callback_state = callback_state;

	worker = thread(&UsbWorker::work, this, deviceId, baud, deadAfterMs);
}

UsbWorker::~UsbWorker()
{
	stop = true;
	worker.join();
}

bool UsbWorker::Send(string data)
{
	if (!sendBEmpty) return false;

	msendbuf.lock();
	if (sendBEmpty)
	{
		strncpy(_sendBuffer, data.c_str(), sizeof(_sendBuffer));
		sendBEmpty = false;
	}
	msendbuf.unlock();

	return true;
}

useconds_t UsbWorker::sleepForBaudAndBytes(int baud, int bufferSize)
{
	unsigned int bitspersec = 0;

	switch (baud)
	{
	case B300:
		bitspersec = 300;
		break;
	case B600:
		bitspersec = 600;
		break;
	case B1200:
		bitspersec = 1200;
		break;
	case B1800:
		bitspersec = 1800;
		break;
	case  B2400:
		bitspersec = 2400;
		break;
	case  B4800:
		bitspersec = 4800;
		break;
	case  B9600:
		bitspersec = 9600;
		break;
	case  B19200:
		bitspersec = 19200;
		break;
	case B38400:
		bitspersec = 38400;
		break;
	case  B57600:
		bitspersec = 57600;
		break;
	case  B115200:
		bitspersec = 115200;
		break;
	default:
		bitspersec = 300;
		break;
	}

	unsigned int numofbits = bufferSize * 8;
	double secondforbits = (double)numofbits / (double)bitspersec;
	return (useconds_t)(secondforbits * 1000 * 1000);
}

void UsbWorker::work(int deviceId, unsigned int baud, int deadAfterMs)
{
	char sendBuffer[SEND_BUFFER_SIZE];

	bool alive = false;
	bool error = false;
	int fd = -1;
	int len;

	char ttyDevs[][20] =
	{
		{"/dev/ttyACM0"},
		{"/dev/ttyUSB0"},
		{"/dev/ttyACM1"},
		{"/dev/ttyUSB1"},
		{"/dev/ttyACM2"},
		{"/dev/ttyUSB2"},
		{"/dev/ttyACM3"},
		{"/dev/ttyUSB3"},
		{"/dev/ttyACM4"},
		{"/dev/ttyUSB4"}
	};
	int ttyDevsNum = ((int)(sizeof(ttyDevs) / sizeof(ttyDevs)[0]));
	int ttycount = -1;

	struct termios tty;

	while (!stop)
	{
		if (bdevtrying)
		{
			if (ttycount == ttyDevsNum)
			{
				bdevtrying = false;
				mdevtrying.unlock();
			}
		}
		else
		{
			mdevtrying.lock();
			bdevtrying = true;
		}

		ttycount = (ttycount + 1) % ttyDevsNum;

		error = (fd = open(ttyDevs[ttycount], O_RDWR | O_NONBLOCK | O_EXCL | O_NOCTTY)) < 0;

		if (!error)
		{
			error = !isatty(fd);
		}

		if (fd > 0)
		{
			errno = 0;

			error = tcgetattr(fd, &tty) != 0; // POSIX states that the struct passed to tcsetattr() must have been initialized with a call to tcgetattr()       

			if (!error)
			{
				error = cfsetspeed(&tty, baud) != 0;
			}

			if (!error)
			{
				cfmakeraw(&tty);
				tty.c_cflag |= CLOCAL;
				tty.c_cflag |= HUPCL;
			}

			if (!error)
			{
				error = (tcsetattr(fd, TCSANOW, &tty) != 0); // save settings
			}

			if (!error)
			{
				steady_clock::time_point data_time_point; // = steady_clock::time_point::min();
				steady_clock::time_point check_time_point;

				int device;
				int nr, nw, ns;
				alive = false;
				useconds_t sleepforbytes = sleepForBaudAndBytes(baud, RECV_BUFFER_SIZE);

				tcflush(fd, TCIOFLUSH);
				usleep(deadAfterMs * 1000);

				char recvChar = '\0';
				int state = 0;

				char idstr[10];
				int idstrc = 0;
				char dtstr[RECV_BUFFER_SIZE];
				int dtstrc = 0;

				data_time_point = steady_clock::now();

				while (!error && !stop)
				{
					nr = read(fd, &recvChar, 1);

					if (nr == 1)
					{
						data_time_point = steady_clock::now();

						switch (state)
						{
						case -1:
							error = true;
							break;
						case 0: // find id start
							if (recvChar == DEVICEID_S)
							{
								state = 1;
								idstrc = 0;
							}
							break;
						case 1: // find id end and check deviceId
							if (idstrc > sizeof(idstr) - 1)
							{
								state = -1; // error bad id
							}
							else
							{
								if (recvChar == DEVICEID_E)
								{
									if (idstrc > 0)
									{
										idstr[idstrc] = '\0';
										int did = (int)strtol(idstr, NULL, 0);
										if (did == deviceId)
										{
											dtstrc = 0;
											state = 2;
										}
										else
										{
											state = -1; // error wrong id
										}
									}
									else
									{
										state = 0; // only id sent, read next
									}
								}
								else
								{
									idstr[idstrc] = recvChar;
									idstrc++;
								}
							}
							break;
						case 2: // find line and printable data
							if (recvChar > 0x1F && recvChar < 0x7F)
							{
								dtstr[dtstrc] = recvChar;
								dtstrc++;
							}
							else if (recvChar == '\r' || recvChar == '\n')
							{
								dtstr[dtstrc] = '\0';

								if (!alive)
								{
									if (_callback_state != NULL)
									{
										_callback_state(ALIVE);
									}

									alive = true;

									if (bdevtrying)
									{
										bdevtrying = false;
										mdevtrying.unlock();
									}
								}

								if (_callback_data != NULL)
								{
									if (strlen(dtstr) > 0)
									{
										memset(_recvBuffer, 0, sizeof(_recvBuffer));
										strncpy(_recvBuffer, dtstr, strlen(dtstr));
										_callback_data(_recvBuffer);
									}
								}

								state = 0;
							}
							break;
						default:
							break;
						}

						if (!error)
						{
							data_time_point = steady_clock::now();

							if (!sendBEmpty)
							{
								msendbuf.lock();
								memcpy(sendBuffer, _sendBuffer, sizeof(sendBuffer));
								msendbuf.unlock();

								len = strnlen(sendBuffer, sizeof(sendBuffer));

								nw = write(fd, &sendBuffer, len);

								error = nw != len;

								if (!error)
								{
									sendBEmpty = true;
								}
								else
								{
									break;
								}
							}

							data_time_point = steady_clock::now();
						}
					}
					else
					{
						check_time_point = steady_clock::now();
						auto duration = duration_cast<milliseconds>(check_time_point - data_time_point).count();

						if (duration > deadAfterMs)
						{
							error = true;
						}
						else
						{
							usleep(sleepforbytes);
						}
					}

					if (error && alive)
					{
						if (_callback_state != NULL)
						{
							_callback_state(DEAD);
						}
					}
				}
			}

			if (error)
			{
				close(fd);
				fd = -1;
			}
		}
	}
}