@objc(MWMTutorialBlurView)
class TutorialBlurView: UIVisualEffectView, ITutorialView {
  var targetView: UIView?
  private var maskPath: UIBezierPath?
  private var maskLayer = CAShapeLayer()
  private let layoutView = UIView(frame: CGRect(x: -100, y: -100, width: 0, height: 0))

  private func setup() {
    maskLayer.fillRule = kCAFillRuleEvenOdd
    layer.mask = maskLayer
    layoutView.translatesAutoresizingMaskIntoConstraints = false
    layoutView.isUserInteractionEnabled = false
    contentView.addSubview(layoutView)
  }

  override init(effect: UIVisualEffect?) {
    super.init(effect: effect)
    setup()
  }

  required init?(coder aDecoder: NSCoder) {
    super.init(coder: aDecoder)
    setup()
  }

  override func point(inside point: CGPoint, with event: UIEvent?) -> Bool {
    return maskPath?.contains(point) ?? super.point(inside: point, with: event)
  }

  override func didMoveToSuperview() {
    super.didMoveToSuperview()
    if superview != nil {
      targetView?.centerXAnchor.constraint(equalTo: layoutView.centerXAnchor).isActive = true
      targetView?.centerYAnchor.constraint(equalTo: layoutView.centerYAnchor).isActive = true
    }
  }

  override func layoutSubviews() {
    super.layoutSubviews()

    let targetCenter = self.layoutView.center
    let r: CGFloat = 40
    let targetRect = CGRect(x: targetCenter.x - r, y: targetCenter.y - r, width: r * 2, height: r * 2)
    maskPath = UIBezierPath(rect: self.bounds)
    maskPath!.append(UIBezierPath(ovalIn: targetRect))
    maskPath!.usesEvenOddFillRule = true
    maskLayer.path = self.maskPath!.cgPath

    let pulsationPath = UIBezierPath(rect: self.bounds)
    pulsationPath.append(UIBezierPath(ovalIn: targetRect.insetBy(dx: -10, dy: -10)))
    pulsationPath.usesEvenOddFillRule = true
    addPulsation(pulsationPath)
  }

  func animateSizeChange(_ duration: TimeInterval) {
    layer.mask = nil
    DispatchQueue.main.asyncAfter(deadline: .now() + duration) {
      self.layer.mask = self.maskLayer
      self.setNeedsLayout()
    }
  }

  func animateFadeOut(_ duration: TimeInterval, completion: @escaping () -> Void) {
    UIView.animate(withDuration: duration, animations: {
      self.effect = nil
      self.contentView.alpha = 0
    }) { _ in
      completion()
    }
  }

  func animateAppearance(_ duration: TimeInterval) {
    contentView.alpha = 0
    UIView.animate(withDuration: duration) {
      self.contentView.alpha = 1
      self.effect = UIBlurEffect(style: UIColor.isNightMode() ? .light : .dark)
    }
  }

  private func addPulsation(_ path: UIBezierPath) {
    let animation = CABasicAnimation(keyPath: "path")
    animation.duration = kDefaultAnimationDuration
    animation.fromValue = self.maskLayer.path
    animation.toValue = path.cgPath
    animation.autoreverses = true
    animation.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
    animation.repeatCount = 2

    let animationGroup = CAAnimationGroup()
    animationGroup.duration = 3
    animationGroup.repeatCount = Float(Int.max)
    animationGroup.animations = [animation]

    maskLayer.add(animationGroup, forKey: "path")
  }
}
