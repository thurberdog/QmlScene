
#include "beep.h"


/**
 * @brief Beep::init
 * @return
 */
bool Beep::init()
{

    ct = QTime::currentTime ();
    qDebug() << __FUNCTION__ << __LINE__ << ct << "Beep init called";
    m_open = true;
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    returnCode =  execute("aplay -L ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current audio devices: " << returnCode;
    return true;
}

/**
 * @brief Beep::init
 * @param frequency
 * @param duration
 * @return
 */
bool Beep::init(const int frequency, const int duration)
{
    qDebug()<< QTime::currentTime() << __FUNCTION__ << __LINE__ << "Beep init called with frequency:"+frequency << "and duration:"<<duration;
    m_frequency = frequency;
    m_duration = duration;
    qDebug() << QTime::currentTime()<< "[QML] beeper frequency "<< frequency << " and duration " << duration <<" set";
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    return true;
}

/**
 * @brief Beep::duration
 * @return
 */

int Beep::duration()
{
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    return m_duration;
}

/**
 * @brief Beep::frequency
 * @return
 */
int Beep::frequency()
{
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    return m_frequency;
}

/**
 * @brief Beep::isSoundCard
 * @return
 */
bool Beep::isSoundCard()
{
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    QString test = execute("aplay -l");
    if (test.indexOf("Dummy") >= 0)
        return false;
    else
        return true;

}

/**
 * @brief Beep::setVolume
 * @param volume
 */

void Beep::setVolume(int volume)

{
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Pre setvolume PCM settings: " << returnCode;
    if (volume >=0 && volume <= 100)
    {
        m_volume = volume;
        QString output = execute(QString("amixer set PCM ").append(QString::number(volume).append("%")));
        qDebug()<< QTime::currentTime()<<__FUNCTION__<<__LINE__ << "[QML]" << output;
    }
    else
        qDebug() << QTime::currentTime()<< "[QML] amixer error: volume must be set between 0 and 100";
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Post setvolume PCM settings: " << returnCode;
}

/**
 * @brief Beep::volume
 * @return
 */
int Beep::volume()
{
    return m_volume;
}


/**
 * @brief Beep::deinit
 */
void Beep::deinit()
{

    // check if we need to close the sound card
    if (isOpen() && m_wavePtr) {
        snd_pcm_close(m_playbackHandle);
        m_open = false;
        qDebug()<< QTime::currentTime()<<__FUNCTION__<<__LINE__  << "[QML] sound card closed";
    }

}

/**
 * @brief Beep::isOpen
 * @return
 */
bool Beep::isOpen()
{
    return m_open;
}

/**
 * @brief Beep::openwave
 * @param path
 * @return
 */
bool Beep::openwave(const QString &path)
{

    m_waveFile = path;
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << ct << "wav file loaded:"+path;
    returnCode =  execute("amixer get PCM");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    returnCode =  execute("aplay -L ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current audio devices: " << returnCode;
    return true;
}

/**
 * @brief Beep::play_error_recover
 * @return
 */

int Beep::play_error_recover(int error_code)
{
    // Recover the stream state from an error or suspend.
    //Returns:
    // 0 when error code was handled successfuly,
    // otherwise a negative error code
    return snd_pcm_recover(m_playbackHandle,        // pcm	PCM handle
                           error_code,              // error number
                           0);                      // silent	do not print error reason
}

int Beep::write_wave()
{
    //Write interleaved frames to a PCM.
    //Returns:
    // a positive number of frames actually written
    // otherwise a negative error code
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
            return snd_pcm_writei(m_playbackHandle,      //pcm	PCM handle
                                  m_wavePtr,     //buffer	frames containing buffer
                                  m_waveSize);   //size	frames to be written

}

/**
 * @brief Beep::play
 */
void Beep::play()
{
    returnCode =  execute("aplay -v "+m_waveFile);
    qDebug()<<"aplay return code: "<< returnCode;
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "finished playing wave file";
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "PCM settings after playing file: " << returnCode;
}

/**
 * @brief Beep::play
 * @param frequency
 * @param duration
 */
void Beep::play(const int frequency, const int duration)
{
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    execute(QString("beep -f ").append(QString::number(frequency).append(" -d ").append(QString::number(duration))));
}

/**
 * @brief Beep::execute
 * @param command
 * @return
 */
QString Beep::execute(QString command)
{
    QProcess p(this);
    p.setProcessChannelMode(QProcess::MergedChannels);
    qDebug()<<QTime::currentTime() << "executing " << command << "\n";

    p.start(command);

    QByteArray data;

    while(p.waitForReadyRead())
        data.append(p.readAll());

    return QString::fromLatin1(data.data());
}

/**
 * @brief Beep::compareID
 * @param id
 * @param ptr
 * @return
 */

unsigned char Beep::compareID(const unsigned char *id, unsigned char *ptr)
{
    unsigned char i = 4;

    while (i--)
    {
        if ( *(id)++ != *(ptr)++ ) return(0);
    }
    return(1);
}

/**
 * @brief Beep::loadWaveFile
 * @param fn
 * @return
 */

bool Beep::loadWaveFile(const char *fn)
{
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    FILE_head head;
    int inHandle;

    if ((inHandle = open(fn, O_RDONLY)) == -1)
    {
        qDebug()<<QTime::currentTime()<<__FUNCTION__<<__LINE__  << "[QML] could not open wave file:" << fn ;
        return false;
    }
    else
    {
        // Read in IFF File header
        if (read(inHandle, &head, sizeof(FILE_head)) == sizeof(FILE_head))
        {
            // Is it a RIFF and WAVE?
            if (!compareID(&Riff[0], &head.ID[0]) || !compareID(&Wave[0], &head.Type[0]))
            {
                close(inHandle);
                qDebug()<<QTime::currentTime()<<__FUNCTION__<<__LINE__  << "[QML] " << fn << "is not a wave file.";
                return false;
            }

            // Read in next chunk header
            while (read(inHandle, &head, sizeof(CHUNK_head)) == sizeof(CHUNK_head))
            {
                // ============================ Is it a fmt chunk? ===============================
                if (compareID(&Fmt[0], &head.ID[0]))
                {
                    FORMAT	format;

                    // Read in the remainder of chunk
                    if (read(inHandle, &format.wFormatTag, sizeof(FORMAT)) != sizeof(FORMAT)) break;

                    // Can't handle compressed WAVE files
                    if (format.wFormatTag != 1)
                    {
                        close(inHandle);
                        qDebug()<<QTime::currentTime()<<__FUNCTION__<<__LINE__  << "[QML] compressed wave file is not supported";
                        return false;
                    }

                    m_waveBits = (unsigned char)format.wBitsPerSample;
                    m_waveRate = (unsigned short)format.dwSamplesPerSec;
                    m_waveChannels = format.wChannels;
                }

                // ============================ Is it a data chunk? ===============================
                else if (compareID(&Data[0], &head.ID[0]))
                {
                    // Size of wave data is head.Length. Allocate a buffer and read in the wave data
                    if (!(m_wavePtr = (unsigned char *)malloc(head.Length)))
                    {
                        close(inHandle);
                        qDebug()<<QTime::currentTime() <<__FUNCTION__<<__LINE__ << "[QML] wave file won't fit in RAM";
                        return false;
                    }

                    if (read(inHandle, m_wavePtr, head.Length) != head.Length)
                    {
                        close(inHandle);
                        free(m_wavePtr);
                        return false;
                    }

                    // Store size (in frames)
                    m_waveSize = (head.Length * 8) / ((unsigned int)m_waveBits * (unsigned int)m_waveChannels);

                    close(inHandle);
                    break;
                }

                // ============================ Skip this chunk ===============================
                else
                {
                    if (head.Length & 1) ++head.Length;  // If odd, round it up to account for pad byte
                    lseek(inHandle, head.Length, SEEK_CUR);
                }
            }
        }
    }

    qDebug()<<QTime::currentTime()<<"[QML] beeper wave file loaded "<< fn;
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    return true;

}

void Beep::update()
{
//    returnCode =  execute("aplay -L");

//    returnCode =  execute("amixer get PCM");
    returnCode =  execute("amixer set PCM 0%");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "about to play wave file:" +m_waveFile + " with the following settings: " << returnCode;
    returnCode =  execute("aplay "+m_waveFile);
    returnCode =  execute("amixer set PCM 80%");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "about to play wave file:" +m_waveFile + " with the following settings: " << returnCode;

    timer->start(1000);
}

/**
 * @brief Beep::Beep
 * @param parent
 */
Beep::Beep(QObject *parent) :
    QObject(parent)
{
    m_wavePtr = NULL;
    m_open = false;
    ct = QTime::currentTime ();
    qDebug() << __FUNCTION__ << __LINE__ << ct << "Beep object created";
    returnCode =  execute("amixer get PCM ");
    qDebug() <<QTime::currentTime()<< __FUNCTION__ << __LINE__ << "Current PCM settings: " << returnCode;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

}

/**
 * @brief Beep::~Beep
 */
Beep::~Beep()
{
    deinit();
    if (m_wavePtr)
        delete(m_wavePtr);
}
