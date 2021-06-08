/**
 * Function to create chartjs tooltip element
 * @param {Object} payload.tooltipModel - chartjs tooltipModel
 * @param {String} payload.tooltipId - tooltipId. Use to remove the tooltip when chart instance is destroyed
 * @param {Object} payload.position -  Chart position
 * @param {String} payload.className - Custom class name for tooltip element
 * @param {Boolean} payload.alignTooltipToLeft - To either align tooltip to the right or left.
 * If not provided, it aligns to center
 */
function createTooltipEle({ tooltipModel, tooltipId, position, className, alignTooltipToLeft }) {
    let tooltipEl = document.getElementById(tooltipId)
    // Create element on first render
    if (!tooltipEl) {
        tooltipEl = document.createElement('div')
        tooltipEl.id = tooltipId
        tooltipEl.className = [`chartjs-tooltip shadow-drop ${className}`]
        tooltipEl.innerHTML = '<table></table>'
        document.body.appendChild(tooltipEl)
    }

    // Hide if no tooltip
    if (tooltipModel.opacity === 0) {
        tooltipEl.style.opacity = 0
        return
    }

    // Set caret Position
    tooltipEl.classList.remove(
        'above',
        'below',
        'no-transform',
        'chartjs-tooltip--transform-left',
        'chartjs-tooltip--transform-right',
        'chartjs-tooltip--transform-center'
    )
    if (tooltipModel.yAlign) tooltipEl.classList.add(tooltipModel.yAlign)
    else tooltipEl.classList.add('no-transform')

    if (typeof alignTooltipToLeft === 'boolean') {
        if (alignTooltipToLeft) tooltipEl.classList.add('chartjs-tooltip--transform-left')
        else tooltipEl.classList.add('chartjs-tooltip--transform-right')
    } else tooltipEl.classList.add('chartjs-tooltip--transform-center')

    // Display, position, and set styles for font
    tooltipEl.style.opacity = 1
    tooltipEl.style.position = 'absolute'
    tooltipEl.style.left = `${position.left + tooltipModel.caretX}px`
    tooltipEl.style.top = `${position.top + tooltipModel.caretY + 10}px`
    tooltipEl.style.fontFamily = tooltipModel._bodyFontFamily
    tooltipEl.style.fontStyle = tooltipModel._bodyFontStyle
    tooltipEl.style.padding = `${tooltipModel.yPadding}px ${tooltipModel.xPadding}px`
    tooltipEl.style.pointerEvents = 'none'
    return tooltipEl
}

/**
 * Custom tooltip to show tooltip for multiple datasets. X axis value will be used
 * as the title of the tooltip. Body of the tooltip is generated by using
 * tooltipModel.body
 * @param {Object} payload.tooltipModel - chartjs tooltipModel
 * @param {String} payload.tooltipId - tooltipId. Use to remove the tooltip when chart instance is destroyed
 * @param {Object} payload.position -  chart canvas position (getBoundingClientRect)
 */
export function streamTooltip({ tooltipModel, tooltipId, position, alignTooltipToLeft }) {
    // Tooltip Element
    let tooltipEl = createTooltipEle({ tooltipModel, tooltipId, position, alignTooltipToLeft })
    // Set Text
    if (tooltipModel.body) {
        let titleLines = tooltipModel.title || []
        let bodyLines = tooltipModel.body.map(item => item.lines)

        let innerHtml = '<thead>'

        titleLines.forEach(title => {
            innerHtml += '<tr><th>' + title + '</th></tr>'
        })
        innerHtml += '</thead><tbody>'

        bodyLines.forEach((body, i) => {
            let colors = tooltipModel.labelColors[i]
            let style = 'background:' + colors.borderColor
            style += '; border-color:' + colors.borderColor
            style += '; border-width: 2px;margin-right:4px'
            let span = '<span class="chartjs-tooltip-key" style="' + style + '"></span>'
            innerHtml += '<tr><td>' + span + body + '</td></tr>'
        })
        innerHtml += '</tbody>'

        let tableRoot = tooltipEl.querySelector('table')
        tableRoot.innerHTML = innerHtml
    }
}

/**
 * Rendering object tooltip for a single dataset by using provided dataPointObj
 * @param {Object} payload.tooltipModel - chartjs tooltipModel
 * @param {String} payload.tooltipId - tooltipId. Use to remove the tooltip when chart instance is destroyed
 * @param {Object} payload.position -  chart canvas position (getBoundingClientRect)
 * @param {Object} payload.dataPointObj - label object
 */
export function objectTooltip({
    tooltipModel,
    tooltipId,
    position,
    dataPointObj,
    alignTooltipToLeft,
}) {
    // Tooltip Element
    let tooltipEl = createTooltipEle({
        tooltipModel,
        tooltipId,
        position,
        alignTooltipToLeft,
        className: 'query-editor-chart-tooltip',
    })
    // Set Text
    if (tooltipModel.body) {
        let innerHtml = '<tbody>'
        Object.keys(dataPointObj).forEach(key => {
            if (key !== 'x' && key !== 'y') {
                innerHtml += `
                <tr>
                    <td class="color text-small-text"> ${key}</td>
                    <td class="color text-navigation">
                        ${dataPointObj[key]}
                    </td>
                </tr>`
            }
        })
        innerHtml += '</tbody>'
        let tableRoot = tooltipEl.querySelector('table')
        tableRoot.innerHTML = innerHtml
    }
}