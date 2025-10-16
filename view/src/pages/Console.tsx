// Copyright mzoesch. All rights reserved.

import {useEffect, useRef, useState} from 'react';
import {getStoredEngineUrl} from "../Common.tsx";

let jafgConsoleInitialized = false

class Message {
  public author: string
  public verbosity: string
  public content: string

  constructor(author: string, verbosity: string, content: string) {
    this.author = author
    this.verbosity = verbosity
    this.content = content
  }
}

function JafgConsole() {
  const [lastFetchedLogId, setLastFetchedLogId] = useState<number>(-1)
  const [logs, setLogs] = useState<Message[]>([])

  const containerRef = useRef<HTMLDivElement | null>(null)

  const scrollToEnd = () => {
    const container = containerRef.current
    if (container) {
      container.scrollTop = container.scrollHeight
    }
  }
  const scrollToEndDelayed = () => {
    setTimeout(() => { scrollToEnd() }, 50)
  }

  const scrollToEndIfNear = () => {
    const container = containerRef.current
    if (!container) {
      return
    }
    const distanceFromBottom = container.scrollHeight - container.scrollTop - container.clientHeight
    if (distanceFromBottom <= 100) {
      container.scrollTop = container.scrollHeight
    }
  }
  const scrollToEndIfNearDelayed = () => {
    setTimeout(() => { scrollToEndIfNear() }, 50)
  }

  const fetchNewLogs = (alwaysScrollToEnd: boolean, id: number | null = null) => {
    const url = getStoredEngineUrl()
    if (url.length == 0) {
      setLogs((prevLogs) => [...prevLogs, new Message('ERR', 'ERROR', 'Engine URL not configured.')])
      if (alwaysScrollToEnd) {
        scrollToEndDelayed()
      } else {
        scrollToEndIfNearDelayed()
      }
      return
    }

    if (id === null) {
      id = lastFetchedLogId + 1
    }

    console.log(`Fetching logs starting at ID ${id} from ${url}/logs?id=${id}`)
    fetch(`${url}/logs?id=${id}`, {
      method: 'GET',
      headers: {'Content-Type': 'application/json'},
    })
      .then(async (res) => {
        if (!res.ok) {
          setLogs((prevLogs) => [...prevLogs, new Message('ERR', 'ERROR', `Server returned status ${res.status} ${res.statusText}`)])
          return
        }

        const data = await res.json()
        if (!data || !data.logs || !Array.isArray(data.logs)) {
          setLogs((prevLogs) => [...prevLogs, new Message('ERR', 'ERROR', 'Invalid response from server.')])
          return
        }

        if (data.logs.length === 0) {
          return
        }

        data.logs.sort((a: { id: number; }, b: { id: number; }) => a.id - b.id)
        if (data.logs.length > 1) {
          if (lastFetchedLogId < data.logs[data.logs.length - 1].id) {
            setLastFetchedLogId(data.logs[data.logs.length - 1].id)
          }
        }

        setLogs((prevLogs) => [...prevLogs,
          ...data.logs.map((log: { id: number; level: string; message: string; }) => new Message('SRV', log.level, log.message))])

        if (alwaysScrollToEnd) {
          scrollToEndDelayed()
        } else{
          scrollToEndIfNearDelayed()
        }
      })
      .catch((err) => {
        console.error(err)
        setLogs((prevLogs) => [...prevLogs, new Message('ERR', 'ERROR', `${err}`)])
        if (alwaysScrollToEnd) {
          scrollToEndIfNearDelayed()
        }
      })
  }

  const getLogClass = (verbosity: string) => {
    switch (verbosity || '') {
      // SVR levels
      case 'Fatal': return 'text-red-950'
      case 'Error': return 'text-red-400'
      case 'Warning': return 'text-yellow-300'
      case 'Info': return 'text-gray-200'
      case 'Verbose': return 'text-gray-500'
      case 'Trace': return 'text-gray-700'
      // ~SVR levels

      // View verbosities
      case 'ERROR': return 'text-red-400'
      case 'WARN': return 'text-yellow-300'
      case 'INFO': return 'text-green-300'
      case 'DEBUG': return 'text-sky-300'
      case 'TRACE': return 'text-gray-300'
      // ~View verbosities

      default:
      {
        console.error(`Unknown verbosity level: ${verbosity}`)
        return 'text-red-400'
      }
    }
  }

  useEffect(() => {
    if (jafgConsoleInitialized) {
      return
    }
    jafgConsoleInitialized = true

    console.log("[YOU]: Initializing console by requesting previous logs...")
    setLogs([new Message('YOU', 'INFO', 'Initializing console by requesting previous logs...')])
    fetchNewLogs(true)
  }, [])

  return (
    <div className='flex-grow flex flex-col gap-2'>
      <div className='flex gap-2 rounded-md border border-white bg-teal-950'>
        <button className='bg-black/20 p-2 rounded-md' onClick={() => { }}>Verbosities</button>
        <div className='flex-grow'/>
        <button className='bg-black/20 p-2 rounded-md' onClick={() => { setLastFetchedLogId(-1); setLogs([]); fetchNewLogs(true, 0) }}>Refetch</button>
        <button className='bg-black/20 p-2 rounded-md' onClick={() => { setLogs([]) }}>Clear</button>
        <button className='bg-black/20 p-2 rounded-md' onClick={() => { scrollToEndDelayed() }}>Scroll To End</button>
      </div>
      <div className='flex flex-col rounded-md border border-white h-96 bg-teal-950/10 flex-grow'>
        <div ref={containerRef} className='font-mono p-2 flex-grow overflow-y-auto'>
          {logs.length === 0
            ? (<div/>)
            : (logs.map((log, idx) => (
                <div key={idx} className={`whitespace-pre-wrap text-left ${getLogClass(log.verbosity)}`}>
                  [{log.author}]: {log.content}
                </div>
              )))
          }
        </div>
        <hr/>
        <div>
          <form onSubmit={(e) => {
            e.preventDefault()
            const input = (e.target as HTMLFormElement)[0] as HTMLInputElement
            const command = input.value.trim()
            input.value = ''

            if (command.length == 0) {
              return
            }

            console.log(`[YOU]: ${command}`)
            setLogs((prevLogs) => [...prevLogs, new Message('YOU', 'INFO', command)])

            setTimeout(() => { scrollToEndIfNear() }, 50)

            const url = getStoredEngineUrl()
            if (url.length == 0) {
              setLogs((prevLogs) => [...prevLogs, new Message('ERR', 'ERROR', 'Engine URL not configured.')])
              return
            }

            fetch(url, {
              method: 'POST',
              headers: {'Content-Type': 'application/json'},
              body: JSON.stringify({command})
            })
              .then(async (res) => {
                if (!res.ok) {
                  setLogs((prevLogs) => [...prevLogs, new Message('ERR', 'ERROR', `Server returned status ${res.status} ${res.statusText}`)])
                  return
                }

                const data = await res.json()
                if (!data || !data.response) {
                  setLogs((prevLogs) => [...prevLogs, new Message('ERR', 'ERROR', 'Invalid response from server.')])
                  return
                }
                setLogs((prevLogs) => [...prevLogs, new Message('SRV', 'INFO', data.response)])

                setTimeout(() => { scrollToEndIfNear() }, 50)
              })
              .catch((err) => {
                console.error(err)
                setLogs((prevLogs) => [...prevLogs, new Message('ERR', 'ERROR', `${err}`)])
              })
          }}>
            <input className='bg-black/20 w-full font-mono p-2' type='text' placeholder='Server command...'/>
          </form>
        </div>
      </div>
    </div>
  )
}

export default function ConsolePage() {
  return (
    <div className='flex flex-col p-4 w-full'>
      <div>
        <h1 className='text-left font-bold underline'>Console</h1>
      </div>
      <div className='mt-4 flex flex-col flex-grow'>
        <JafgConsole />
      </div>
    </div>
  )
}
