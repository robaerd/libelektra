/**
 * @file
 *
 * @brief simple editable text field for tree items
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

import React, { Component } from 'react'

import TextField from 'material-ui/TextField'

import validateType from './validateType'
import debounce from '../../debounce'
import { fromElektraBool } from '../../../utils'

const DebouncedTextField = debounce(TextField)

export default class SimpleTextField extends Component {
  constructor (props) {
    super(props)
    this.state = { value: false, error: false }
  }

  render () {
    const { id, value, meta, label, debounce = true, onChange, onError } = this.props
    const val = this.state.value === false ? value : this.state.value
    const comp = debounce ? DebouncedTextField : TextField

    return (
      <div draggable="true" onDragStart={e => e.preventDefault()}>
        {React.createElement(comp, {
          id,
          value: val,
          errorText: this.state.error,
          hintText: (meta && meta.example) ? `e.g. ${meta.example}` : false,
          onChange: debounce
            ? value => this.setState({ value })
            : evt => (evt && evt.target && evt.target.value) && onChange(evt.target.value),
          onDebounced: debounce && (currentValue => {
            const validationError = validateType(meta, currentValue)
            if (validationError) {
              if (typeof onError === 'function') onError(validationError)
              return this.setState({ error: validationError })
            } else {
              if (typeof onError === 'function') onError(false)
              this.setState({ error: false })
            }
            onChange(currentValue)
          }),
          disabled: fromElektraBool(meta && meta.readonly),
          floatingLabelText: label,
          floatingLabelFixed: !!label,
        })}
      </div>
    )
  }
}