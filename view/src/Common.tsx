// Copyright mzoesch. All rights reserved.

const ENGINE_URL_STORAGE_KEY = 'engine_url'

export function getStoredEngineUrl(): string {
  if (typeof window === 'undefined') {
    return ''
  }

  if (!localStorage) {
    return ''
  }

  const item = localStorage.getItem(ENGINE_URL_STORAGE_KEY)

  if (!item) {
    return ''
  }

  return item
}
