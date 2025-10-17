// Copyright mzoesch. All rights reserved.

import {useEffect, useRef, useState} from 'react';
import {MultiSelect} from "../components/MultiSelect.tsx";
import {
  LogCategory,
  hasStoredEntry,
  storeBoolean, getStoredBoolean,
  storeNumber, getStoredNumber,
  ENGINE_URL_STORAGE_KEY,
  LOG_VERBOSITY_FILTERS_STORAGE_KEY, LOG_CATEGORY_FILTERS_STORAGE_KEY,
  LOG_DO_AUTO_FETCH, LOG_AUTO_FETCH_INTERVAL,
  getStoredLogVerbosityFilters, updateStoredLogVerbosityFilters,
  getStoredLogCategoryFilters, updateStoredLogCategoryFilters, getStoredValue,
} from "../lib/Storage.tsx";
import {Menu, MenuButton, MenuItems, Input} from "@headlessui/react";
import {ChevronDownIcon} from "@heroicons/react/16/solid";

let jafgConsoleInitialized = false

class LogMessage {
  public author: string
  public verbosity: string
  public content: string

  constructor(author: string, verbosity: string, content: string) {
    this.author = author
    this.verbosity = verbosity
    this.content = content
  }
}

const verbosityLevels: string[] = ['Fatal', 'Error', 'Warning', 'Info', 'Verbose', 'Trace']
const sortCategories = (a: string, b: string) => {
  if (a === '<Uncategorized>') return -1
  if (b === '<Uncategorized>') return 1
  return a.localeCompare(b)
}

const getLogTextColor = (verbosity: string) => {
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

function JafgConsole() {
  const containerRef = useRef<HTMLDivElement | null>(null)
  const fetchIntervalRef = useRef<number | null>(null)
  const countdownIntervalRef = useRef<number | null>(null)

  const [selectedVerbosities, setSelectedVerbosities] = useState<Array<string>>(hasStoredEntry(LOG_VERBOSITY_FILTERS_STORAGE_KEY)
    ? getStoredLogVerbosityFilters()
    : verbosityLevels)
  const [categories, setCategories] = useState<Array<LogCategory>>(hasStoredEntry(LOG_CATEGORY_FILTERS_STORAGE_KEY)
    ? getStoredLogCategoryFilters()
    : [new LogCategory('<Uncategorized>', true)])

  const [autoFetchEnabled, setAutoFetchEnabled] = useState<boolean>(hasStoredEntry(LOG_DO_AUTO_FETCH)
    ? getStoredBoolean(LOG_DO_AUTO_FETCH)
    : true)
  const [autoFetchInterval, setAutoFetchInterval] = useState<number>(hasStoredEntry(LOG_AUTO_FETCH_INTERVAL)
    ? getStoredNumber(LOG_AUTO_FETCH_INTERVAL)
    : 5)
  const [nextFetchCountdown, setNextFetchCountdown] = useState<number>(autoFetchInterval)

  const [lastFetchedLogId, setLastFetchedLogId] = useState<number>(-1)
  const [logs, setLogs] = useState<LogMessage[]>([])

  const lastFetchedLogIdRef = useRef<number>(lastFetchedLogId)
  useEffect(() => {
    lastFetchedLogIdRef.current = lastFetchedLogId
  }, [lastFetchedLogId])

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

  const setSelectedVerbositiesHandler = (newVerbosities: Array<string>) => {
    setSelectedVerbosities(newVerbosities)
    updateStoredLogVerbosityFilters(newVerbosities)
    scrollToEndDelayed()
  }
  const setCategoriesHandler = (newCategories: Array<string>) => {
    const updatedCategories = categories.map((c) => {
      return newCategories.includes(c.name)
        ? new LogCategory(c.name, true)
        : new LogCategory(c.name, false)
    })
    setCategories(updatedCategories)
    updateStoredLogCategoryFilters(updatedCategories)

    scrollToEndDelayed()
  }

  const fetchNewLogs = (alwaysScrollToEnd: boolean, id: number | null = null) => {
    const url = getStoredValue(ENGINE_URL_STORAGE_KEY)
    if (url.length == 0) {
      setLogs((prevLogs) => [...prevLogs, new LogMessage('ERR', 'ERROR', 'Engine URL not configured.')])
      if (alwaysScrollToEnd) {
        scrollToEndDelayed()
      } else {
        scrollToEndIfNearDelayed()
      }
      return
    }

    if (id === null) {
      id = lastFetchedLogIdRef.current + 1
    }

    console.log(`Fetching logs starting at ID ${id} from ${url}/logs?id=${id}`)
    fetch(`${url}/logs?id=${id}`, {
      method: 'GET',
      headers: {'Content-Type': 'application/json'},
    })
      .then(async (res) => {
        if (!res.ok) {
          setLogs((prevLogs) => [...prevLogs, new LogMessage('ERR', 'ERROR', `Server returned status ${res.status} ${res.statusText}`)])
          return
        }

        const data = await res.json()
        if (!data || !data.logs || !Array.isArray(data.logs)) {
          setLogs((prevLogs) => [...prevLogs, new LogMessage('ERR', 'ERROR', 'Invalid response from server.')])
          return
        }

        if (data.logs.length === 0) {
          return
        }

        data.logs.sort((a: { id: number; }, b: { id: number; }) => a.id - b.id)
        const highestId = data.logs.reduce((max: number, l: { id: number }) => Math.max(max, l.id), lastFetchedLogIdRef.current)
        if (highestId > lastFetchedLogIdRef.current) {
          setLastFetchedLogId(highestId)
          lastFetchedLogIdRef.current = highestId
        }

        for (const log of data.logs) {
          if (!log.message.startsWith('[')) {
            continue
          }
          const endIdx = log.message.indexOf(']')
          if (endIdx === -1) {
            continue
          }
          const category = log.message.substring(1, endIdx)
          if (!categories.map((c) => c.name).includes(category)) {
            categories.push(new LogCategory(category, true))
            categories.sort((a, b) => sortCategories(a.name, b.name))
            setCategories([...categories])
            updateStoredLogCategoryFilters(categories)
          }
        }

        setLogs((prevLogs) => [...prevLogs,
          ...data.logs.map((log: { id: number; level: string; message: string; }) => new LogMessage('SRV', log.level, log.message))])

        if (alwaysScrollToEnd) {
          scrollToEndDelayed()
        } else{
          scrollToEndIfNearDelayed()
        }
      })
      .catch((err) => {
        console.error(err)
        setLogs((prevLogs) => [...prevLogs, new LogMessage('ERR', 'ERROR', `${err}`)])
        if (alwaysScrollToEnd) {
          scrollToEndIfNearDelayed()
        }
      })
  }

  useEffect(() => {
    if (jafgConsoleInitialized) {
      return
    }
    jafgConsoleInitialized = true

    console.log("[YOU]: Initializing console by requesting previous logs...")
    setLogs([new LogMessage('YOU', 'INFO', 'Initializing console by requesting previous logs...')])
    fetchNewLogs(true)
  }, [])

  useEffect(() => {
    if (fetchIntervalRef.current !== null) {
      clearInterval(fetchIntervalRef.current)
      fetchIntervalRef.current = null
    }
    if (countdownIntervalRef.current !== null) {
      clearInterval(countdownIntervalRef.current)
      countdownIntervalRef.current = null
    }

    if (!autoFetchEnabled) {
      return
    }

    let remaining = autoFetchInterval
    setNextFetchCountdown(remaining)
    countdownIntervalRef.current = window.setInterval(() => {
      remaining -= 1
      if (remaining <= 0) {
        remaining = autoFetchInterval
      }
      setNextFetchCountdown(remaining)
    }, 1000)

    fetchIntervalRef.current = window.setInterval(() => {
      fetchNewLogs(false)
    }, autoFetchInterval * 1000)

    return () => {
      if (fetchIntervalRef.current !== null) {
        clearInterval(fetchIntervalRef.current)
        fetchIntervalRef.current = null
      }
      if (countdownIntervalRef.current !== null) {
        clearInterval(countdownIntervalRef.current)
        countdownIntervalRef.current = null
      }
    }
  }, [autoFetchEnabled, autoFetchInterval])

  return (
    <div className='flex-grow flex flex-col gap-2'>
      <div className='flex gap-2 rounded-md border border-white/20 bg-teal-950 p-1'>
        <MultiSelect
          options={verbosityLevels}
          value={selectedVerbosities}
          onChange={setSelectedVerbositiesHandler}
          display="Verbosities"
        />
        <MultiSelect
          options={categories.map((c) => c.name).sort(sortCategories)}
          value={categories.filter((c) => c.selected).map((c) => c.name)}
          onChange={setCategoriesHandler}
          display="Categories"
        />
        <div className='flex-grow'/>
        <button className='bg-black/20 p-2 rounded-md' onClick={() => { setLastFetchedLogId(-1); setLogs([]); fetchNewLogs(true, 0) }}>Refetch</button>
        <Menu>
          <MenuButton className={`inline-flex items-center gap-2
            rounded-md bg-gray-800 px-3 py-1.5 focus:not-data-focus:outline-none data-focus:outline
            data-focus:outline-white data-hover:bg-gray-700 data-open:bg-gray-700
            ${autoFetchEnabled ? 'text-green-500' : 'text-red-500'}
            `}
          >
            Auto Fetch <ChevronDownIcon className='size-4 fill-white/60'/>
          </MenuButton>
          <MenuItems transition anchor='bottom end'
            className='px-2 origin-top-right rounded-sm border border-white/10 bg-slate-800 p-1 text-sm/6 text-white
              flex flex-col gap-1
              transition duration-60 ease-out [--anchor-gap:--spacing(1)] focus:outline-none data-closed:scale-95 data-closed:opacity-0'
          >
            <p>Auto fetch from the server in intervals.</p>
            <hr className='border-white/50'/>
            <div className='flex items-center gap-2'>
              <p>
                Interval (seconds):
              </p>
              <div className='flex items-center gap-2'>
                <Input type='number' min={1} step={1} defaultValue={autoFetchInterval}
                  className='block w-15 rounded-lg border-none bg-white/5 px-2 py-0.5 text-sm/6 text-white
                    focus:not-data-focus:outline-none data-focus:outline-2 data-focus:-outline-offset-2 data-focus:outline-white/25'
                  onChange={(e) => {
                    const val = parseInt(e.target.value)
                    if (isNaN(val)) {
                      return
                    }
                    if (val === 0) {
                      console.warn("Auto fetch interval cannot be zero.")
                      return
                    }
                    if (val < 0) {
                      console.warn("Auto fetch interval cannot be negative.")
                      return
                    }

                    setAutoFetchInterval(val)
                    storeNumber(LOG_AUTO_FETCH_INTERVAL, val)
                  }}
                />
              </div>
              <div className='flex items-center gap-2 ml-3'>
                <Input type='checkbox' checked={autoFetchEnabled} onChange={(e) => {
                  const enabled = e.target.checked
                  setAutoFetchEnabled(enabled)
                  storeBoolean(LOG_DO_AUTO_FETCH, enabled)
                }} />
                Enable
              </div>
            </div>
            {
              autoFetchEnabled
              ? (<>
                <hr className='border-white/50'/>
                <div>Next fetch in {nextFetchCountdown} seconds.</div>
                </>)
              : (<div/>)
            }
          </MenuItems>
        </Menu>
        <button className='bg-black/20 p-2 rounded-md' onClick={() => { setLogs([]) }}>Clear</button>
        <button className='bg-black/20 p-2 rounded-md' onClick={() => { scrollToEndDelayed() }}>Scroll To End</button>
      </div>
      <div className='flex flex-col rounded-md border border-white h-96 bg-teal-950/10 flex-grow'>
        <div ref={containerRef} className='font-mono p-2 flex-grow overflow-y-auto'>
          {logs.length === 0
            ? (<div/>)
            : (logs
              .filter((log) => selectedVerbosities.includes(log.verbosity))
              .filter((log) => {
                const endIdx = log.content.indexOf(']')
                if (endIdx === -1 || !log.content.startsWith('[')) {
                  return categories.find((c) => c.name === '<Uncategorized>')?.selected || false
                }
                const category = log.content.substring(1, endIdx)
                return categories.find((c) => c.name === category)?.selected || false
              })
              .map((log, idx) => (
                <div key={idx} className={`whitespace-pre-wrap text-left ${getLogTextColor(log.verbosity)}`}>
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
            setLogs((prevLogs) => [...prevLogs, new LogMessage('YOU', 'INFO', command)])

            setTimeout(() => { scrollToEndIfNear() }, 50)

            const url = getStoredValue(ENGINE_URL_STORAGE_KEY)
            if (url.length == 0) {
              setLogs((prevLogs) => [...prevLogs, new LogMessage('ERR', 'ERROR', 'Engine URL not configured.')])
              return
            }

            fetch(url, {
              method: 'POST',
              headers: {'Content-Type': 'application/json'},
              body: JSON.stringify({command})
            })
              .then(async (res) => {
                if (!res.ok) {
                  setLogs((prevLogs) => [...prevLogs, new LogMessage('ERR', 'ERROR', `Server returned status ${res.status} ${res.statusText}`)])
                  return
                }

                const data = await res.json()
                if (!data || !data.response) {
                  setLogs((prevLogs) => [...prevLogs, new LogMessage('ERR', 'ERROR', 'Invalid response from server.')])
                  return
                }
                setLogs((prevLogs) => [...prevLogs, new LogMessage('SRV', 'INFO', data.response)])

                setTimeout(() => { scrollToEndIfNear() }, 50)
              })
              .catch((err) => {
                console.error(err)
                setLogs((prevLogs) => [...prevLogs, new LogMessage('ERR', 'ERROR', `${err}`)])
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
    <div className='p-4 flex flex-col flex-grow'>
      <JafgConsole />
    </div>
  )
}
