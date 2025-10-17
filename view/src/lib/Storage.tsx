// Copyright mzoesch. All rights reserved.

export const ENGINE_URL_STORAGE_KEY = 'engine_url'
export const LOG_CATEGORY_FILTERS_STORAGE_KEY = 'log_category_filters'
export const LOG_VERBOSITY_FILTERS_STORAGE_KEY = 'log_verbosity_filters'
export const LOG_DO_AUTO_FETCH = 'log_do_auto_fetch'
export const LOG_AUTO_FETCH_INTERVAL = 'log_auto_fetch_interval_s'

export class LogCategory {
  public name: string
  public selected: boolean

  constructor(name: string, selected: boolean) {
    this.name = name
    this.selected = selected
  }
}

export function storeValue(key: string, value: string): void {
  if (typeof window === 'undefined') {
    return
  }

  if (!localStorage) {
    return
  }

  localStorage.setItem(key, value)

  return;
}

export function getStoredValue(key: string): string {
  if (typeof window === 'undefined') {
    return ''
  }

  if (!localStorage) {
    return ''
  }

  const item = localStorage.getItem(key)

  if (!item) {
    return ''
  }

  return item
}

export function storeBoolean(key: string, value: boolean): void { storeValue(key, value ? 'true' : 'false'); }

export function getStoredBoolean(key: string): boolean { const value = getStoredValue(key); return value === 'true' }

export function storeNumber(key: string, value: number): void { storeValue(key, value.toString()); }

export function getStoredNumber(key: string): number { const value = getStoredValue(key); const parsed = parseFloat(value); return isNaN(parsed) ? 0 : parsed; }

export function storeArray(key: string, array: string[]): void {
  if (typeof window === 'undefined') {
    return
  }

  if (!localStorage) {
    return
  }

  localStorage.setItem(key, JSON.stringify(array))

  return;
}

export function getStoredArray(key: string): string[] {
  if (typeof window === 'undefined') {
    return []
  }

  if (!localStorage) {
    return []
  }

  const item = localStorage.getItem(key)

  if (!item) {
    return []
  }

  try {
    const parsed = JSON.parse(item)
    if (Array.isArray(parsed)) {
      return parsed
    } else {
      console.warn(`Stored item for key ${key} is not an array`)
      return []
    }
  } catch {
    console.warn(`Failed to parse stored item for key ${key}`)
    return []
  }
}

export function deleteStoredEntry(key: string): void {
  if (typeof window === 'undefined') {
    return
  }

  if (!localStorage) {
    return
  }

  localStorage.removeItem(key)

  return;
}

export function hasStoredEntry(key: string): boolean {
  if (typeof window === 'undefined') {
    return false
  }

  if (!localStorage) {
    return false
  }

  return localStorage.getItem(key) !== null
}

export function updateStoredLogVerbosityFilters(filters: string[]): void { storeArray(LOG_VERBOSITY_FILTERS_STORAGE_KEY, filters); }
export function getStoredLogVerbosityFilters(): string[] { return getStoredArray(LOG_VERBOSITY_FILTERS_STORAGE_KEY); }

export function updateStoredLogCategoryFilters(filters: LogCategory[]): void {
  const toStore = filters.map((c) => ({ name: c.name, selected: c.selected }))
  storeArray(LOG_CATEGORY_FILTERS_STORAGE_KEY, toStore.map((c) => JSON.stringify(c)))
}

export function getStoredLogCategoryFilters(): LogCategory[] {
  const stored = getStoredArray(LOG_CATEGORY_FILTERS_STORAGE_KEY)
  const categories: LogCategory[] = []

  stored.forEach((item) => {
    try {
      const parsed = JSON.parse(item)
      if (parsed.name && typeof parsed.selected === 'boolean') {
        categories.push(new LogCategory(parsed.name, parsed.selected))
      }
    } catch {
      console.warn(`Failed to parse stored log category: ${item}`)
    }
  })

  categories.sort((a, b) => {
    if (a.name == '<Uncategorized>') return -1
    if (b.name == '<Uncategorized>') return 1
    return a.name.localeCompare(b.name)
  })

  return categories
}
