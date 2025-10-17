import { useState } from 'react';
import {ChevronDownIcon} from '@heroicons/react/16/solid';

export interface MultiSelectProps<T extends string | number> {
  options: T[];
  value?: T[];
  onChange?: (selected: T[]) => void;
  display?: string;
}

export function MultiSelect<T extends string | number>({
  options,
  value = [],
  onChange,
  display,
  }: MultiSelectProps<T>) {
  const [open, setOpen] = useState(false);

  const toggleOption = (option: T) => {
    const newSelected = value.includes(option)
      ? value.filter((o) => o !== option)
      : [...value, option];
    onChange?.(newSelected);
  };

  return (
    <div className='relative inline-block text-left'>
      <button
        type='button'
        onClick={() => setOpen((o) => !o)}
        className='inline-flex items-center gap-2
        rounded-md bg-gray-800 px-3 py-1.5 focus:not-data-focus:outline-none data-focus:outline
        data-focus:outline-white data-hover:bg-gray-700 data-open:bg-gray-700'
      >
        {display} <ChevronDownIcon className='size-4 fill-white/60'/>
      </button>

      {open && (
        <div className='absolute mt-1 w-full min-w-max bg-slate-800 border rounded shadow-lg z-10 max-h-250 overflow-y-auto'>
          {options.map((option) => (
            <label
              key={String(option)}
              className='flex items-center px-3 py-1 hover:bg-slate-700 cursor-pointer whitespace-nowrap'
            >
              <input
                type='checkbox'
                checked={value.includes(option)}
                onChange={() => toggleOption(option)}
                className='mr-2'
              />
              {option}
            </label>
          ))}
        </div>
      )}
    </div>
  );
}
